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
#ifndef MIDIEVENT_H
#define MIDIEVENT_H

#include "event.h"

class MidiEvent : public Event
{
    unsigned char type;
    unsigned char databyte1;
    unsigned char databyte2;
public:
    static const unsigned char TYPE_CONTROL = 0xB0;
    unsigned char channel;
    MidiEvent() : Event(1, 1, 1) {}
    MidiEvent(Position &position, int databyte1, int databyte2, int type, unsigned char channel);
    MidiEvent(const MidiEvent&);
    friend std::ostream& operator<< (std::ostream &out, MidiEvent &evt);
    void setPosition(int bar, int position, int division) {
        Position(bar, position, division);
    }   
    unsigned char getDatabyte1() {
        return databyte1;
    }
    unsigned char getDatabyte2() {
        return databyte2;
    }
    unsigned char getType() {
        return type;
    }
    uint8_t dataSize();
    void pack(void *buffer);
    void unpack(const uint8_t *buffer);
    bool matches(int type);
    bool matches(int type, int databyte1, int low, int high);
};

#endif // MIDIEVENT_H
