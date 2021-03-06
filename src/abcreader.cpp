/*
 * This file is part of Bipscript.
 *
 * Bipscript is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Bipscript is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Bipscript.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "abcreader.h"
#include "audioengine.h"
#include <iostream>
#include <math.h>

using namespace bipscript;
using namespace midi;

ABCReader *ABCReader::activeParser;

extern "C"
{
// defined in abcmidi
void parse_abc(const char *abc);
void parse_abc_raw(const char *abc, const char *key, const char *length,
              const char *meter, const char *rhythm);

// callbacks defined here
void  mfwrite(int format, int ntracks, int division, FILE *fp);
void mf_write_tempo(long tempo);
int mf_write_meta_event(long delta_time, int type, char *data, int size);
int mf_write_midi_event(long delta_time, int type, int chan, char *data, int size);
void single_note_tuning_change(int key, float midipitch);
// custom callbacks
void add_error(char *s, int lineno, int linepos);
void add_warning(char *s, int lineno, int linepos);
}

long (*Mf_writetrack)(int) = 0;

void  mfwrite(int format, int ntracks, int division, FILE *fp)
{
    ABCReader *parser = ABCReader::getActiveParser();
    parser->startSequence(format, ntracks, division);
    for(int i = 0; i < ntracks; i++) {
        parser->startTrack(i);
        long endspace = (*Mf_writetrack)(i);
    }
}

void  mf_write_tempo(long tempo)
{
    ABCReader::getActiveParser()->writeTempo(tempo);
}

int mf_write_meta_event(long delta_time, int type, char *data, int size)
{
    return ABCReader::getActiveParser()->writeMetaEvent(delta_time, type, data, size);
}

int mf_write_midi_event(long delta_time, int type, int chan, char *data, int size)
{
    return ABCReader::getActiveParser()->writeMidiEvent(delta_time, type, chan, data, size);
}

void single_note_tuning_change(int key, float midipitch)
{
    ABCReader::getActiveParser()->singleNoteTuningChange(key, midipitch);
}

void add_error(char *s, int lineno, int linepos)
{
    ABCReader::getActiveParser()->addError(s, lineno, linepos);
}

void add_warning(char *s, int lineno, int linepos)
{
    ABCReader::getActiveParser()->addWarning(s, lineno, linepos);
}

namespace bipscript {
namespace midi {

std::string ABCReader::error()
{
    ABCError &error = errors.front();
    std::string message("ABC Error [");
    message += std::to_string(error.lineNo);
    message += ":";
    message += std::to_string(error.linePos);
    message += "] ";
    message += error.mesg;
    return message;
}

Pattern *ABCReader::read(const char *abc, const char *key, const char *noteLength) {
    transport::TimeSignature &time = bipscript::AudioEngine::instance().getTimeSignature();
    if(time.isValid()) {
        std::string timeString;
        timeString += std::to_string(time.getNumerator());
        timeString += "/";
        timeString += std::to_string(time.getDenominator());
        return read(abc, key, noteLength, timeString.c_str());
    } else {
        return read(abc, key, noteLength, "4/4");
    }
}

Pattern *ABCReader::read(const char *abc, const char *key, const char *noteLength,
                         const char *meter, const char *rhythm)
{
    activeParser = this;
    errors.clear();
    parse_abc_raw(abc, key, noteLength, meter, rhythm);
    if(errors.size()) {
        throw std::logic_error(error());
    }
    // remove and delete tunes
    Tune *tune = tunes.back();
    while(tunes.size() > 1) {
        tunes.pop_back();
        delete tune;
        tune = tunes.back();
    }
    tunes.pop_back();
    // determine track index
    uint32_t trackIndex = 1;
    if(tune->trackCount() > 1) {
        trackIndex = 2;
    }
    // clone pattern from tune
    Pattern *ret = new Pattern(*tune->track(trackIndex));
    delete tune;
    return ret;
}

Tune *ABCReader::readTune(const char *abc)
{
    activeParser = this;
    errors.clear();
    parse_abc(abc);
    if(errors.size()) {
        throw std::logic_error(error());
    }
    // delete additional tunes
    Tune *tune = tunes.back();
    while(tunes.size() > 1) {
        tunes.pop_back();
        delete tune;
        tune = tunes.back();
    }
    // remove and return first tune
    tunes.pop_back();
    return tune;
}

void ABCReader::startSequence(int format, int ntracks, int division)
{
    // store division
    ticksPerQuarter = division;
    ticksPerBeat = ticksPerQuarter * 4 / beatUnit;
	// create new MidiTune
    tunes.push_back(new Tune(ntracks));
}

void ABCReader::startTrack(uint32_t track) {
    currentPosition = Position(1, 0, 1);
    activeNote.clear();
    activeTrack = track;
}

void ABCReader::writeTempo(long tempo)
{
    // printf("writing tempo:%ld\n", tempo);
}

int ABCReader::writeMetaEvent(long delta_time, int type, char *data, int size)
{
    // printf("got META event type:0x%x delta:%ld\n", type, delta_time);
    if(type == 0x03 && activeTrack == 0) { // title
        currentTune()->setTitle(data); // TODO: should append, may be multiple lines
    }
    else if(type == 0x58) {
        beatsPerBar = data[0];
        beatUnit = pow(2, data[1]);
        ticksPerBeat = ticksPerQuarter * 4 / beatUnit;
        currentTune()->setTimeSignature(beatsPerBar, beatUnit);
    }
    return 0;
}

int ABCReader::writeMidiEvent(long delta_time, int type, int chan, char *data, int size)
{
    // printf("got midi event type:0x%x delta:%ld [%d:%d]\n", type, delta_time, data[0], data[1]);
    currentPosition += Duration(0, delta_time, ticksPerBeat * beatsPerBar);
    if(type == Event::TYPE_NOTE_ON) {
        int key = data[0] | chan << 8;
        activeNote.insert(std::pair<int,Note>(key, Note(data[0], data[1], chan)));
        noteStart[key] = currentPosition;
    } else if(type == Event::TYPE_NOTE_OFF) {
        int key = data[0] | chan << 8;
        auto it = activeNote.find(key);
        if(it != activeNote.end()) {
            Note note = it->second;
            note.setDuration((Duration)currentPosition - (Duration)noteStart[key]);
            currentTune()->addMidiNote(activeTrack, note, noteStart[key]);
            activeNote.erase(key);
        }
    } else {
        addWarning("MIDI event type not yet implemented", 0, 0);
    }
    return 0;
}

void ABCReader::singleNoteTuningChange(int key, float midipitch)
{
}

void ABCReader::addError(const char *mesg, int lineno, int linepos)
{
    errors.push_back(ABCError(mesg, lineno, linepos));
}

void ABCReader::addWarning(const char *mesg, int lineno, int linepos)
{
    if(verbose) {
        std::string message("ABC Warning [");
        message += std::to_string(lineno);
        message += ":";
        message += std::to_string(linepos);
        message += "] ";
        message += mesg;
        std::cerr << message << std::endl;
    }
}

}}