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

#include "midisink.h"

void MidiSink::schedule(Control &cc, int bar, int  position, int  division)
{
    int channel = 0;
    //std::cout << "schedule cc: " << cc.controller << " @ " << bar << ":" << position << "/" << division << std::endl;
    //if(bar >= AudioEngine::instance()->getCurrentBar()) { // move this check lower, into port?s
    Position start(bar, position, division);
    MidiEvent* evt = new MidiEvent(start, cc.controller, cc.value, 0xB0, channel);
    addMidiEvent(evt);
}

void MidiSink::schedule(const Note &note, Position &position, unsigned char channel)
{
    if(channel < 1 || channel > 16) {
        throw std::logic_error("MIDI channel must be between 1 and 16");
    }
    MidiEvent* evt = new MidiEvent(position, note.note, note.velocity, 0x90, channel - 1);
    addMidiEvent(evt);
    Position endPosition = position + note.duration;
    MidiEvent* offevt = new MidiEvent(endPosition, note.note, 0, 0x80, channel - 1);
    addMidiEvent(offevt);
}

void MidiSink::schedule(Pattern &pattern, Position &position, unsigned char channel)
{
    if(channel < 1 || channel > 16) {
        throw std::logic_error("MIDI channel must be between 1 and 16");
    }
    MidiEvent *event = pattern.getFirstEvent();
    if(!event) {
        for(unsigned int i = 0; i < pattern.getSize(); i++) {
            const PatternNote &note = pattern.get(i);
            Position newPosition = position + note.getPosition();
            this->schedule(note.getNote(), newPosition, channel);
        }
    }
    else {
        while(event) {
            MidiEvent *newEvent = new MidiEvent(*event);
            *newEvent += position;
            newEvent->setChannel(channel - 1);
            addMidiEvent(newEvent);
            event = pattern.getNextEvent(event);
        }
    }
}

void MidiSink::schedule(ProgramChange &pc, uint32_t bar, uint32_t position, uint32_t division)
{
    int channel = 0;
    Position start(bar, position, division);
    MidiEvent* evt = new MidiEvent(start, pc.program, 0, 0xC0, channel);
    addMidiEvent(evt);
}
