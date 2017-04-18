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

namespace bipscript {
namespace midi {

void Sink::scheduleNote(const Note &note, Position &position, unsigned char channel)
{
    Event* evt = new Event(position, note.pitch(), note.velocity(), 0x90, channel - 1);
    addMidiEvent(evt);
    Position endPosition = position + note.duration;
    Event* offevt = new Event(endPosition, note.pitch(), 0, 0x80, channel - 1);
    addMidiEvent(offevt);
}

void Sink::schedule(Pattern &pattern, Position &position, unsigned char channel)
{
    if(channel < 1 || channel > 16) {
        throw std::logic_error("MIDI channel must be between 1 and 16");
    }
    for(unsigned int i = 0; i < pattern.size(); i++) {
        const PatternNote &note = pattern.get(i);
        Position newPosition = position + note.getPosition();
        newPosition.setBar(newPosition.getBar() - 1); // pattern bars 1-based
        this->scheduleNote(note.getNoteRef(), newPosition, channel);
    }
}

void Sink::schedule(Message &mesg, Position &position, unsigned char channel)
{
    if(channel < 1 || channel > 16) {
        throw std::logic_error("MIDI channel must be between 1 and 16");
    }
    Event* evt = new Event(position, mesg.byte(0), mesg.byte(1), mesg.type(), channel - 1);
    addMidiEvent(evt);
}

}}