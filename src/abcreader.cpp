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

ABCReader *ABCReader::activeParser;

extern "C"
{
// defined in abcmidi
void parseabc(const char *abc);

// callbacks defined here
void  mfwrite(int format, int ntracks, int division, FILE *fp);
void mf_write_tempo(long tempo);
int mf_write_meta_event(long delta_time, int type, char *data, int size);
int mf_write_midi_event(long delta_time, int type, int chan, char *data, int size);
void single_note_tuning_change(int key, float midipitch);
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

MidiTune *ABCReader::read(const char *abc)
{
    activeParser = this;
    parseabc(abc);
    return tune;
}

void ABCReader::startSequence(int format, int ntracks, int division)
{
    // get division
    tune = new MidiTune(ntracks);
    ticksPerBeat = division;
}

void ABCReader::startTrack(uint32_t track) {
    currentPosition = Position(1,1,1);
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
        tune->setTitle(data);
    }
    else if(type == 0x58) {
        //printf("got time sig %d:%d:%d:%d\n", data[0], data[1], data[2], data[3]);
        uint32_t num = data[0];
        uint32_t denom = pow(2, data[1]);
        beatsPerBar = num / (denom / 4);
        // std::cout << "beats per bar now " << beatsPerBar << std::endl;
    }
    return 0;
}

int ABCReader::writeMidiEvent(long delta_time, int type, int chan, char *data, int size)
{
    // printf("got midi event type:0x%x delta:%ld [%d:%d]", type, delta_time, data[0], data[1]);
    currentPosition += Duration(0, delta_time, ticksPerBeat * beatsPerBar);
    // std::cout << " new position " << currentPosition << std::endl;
    MidiEvent *event = new MidiEvent(currentPosition, data[0], data[1], type, chan);
    tune->addMidiEvent(activeTrack, event);
    return 0;
}

void ABCReader::singleNoteTuningChange(int key, float midipitch)
{
}
