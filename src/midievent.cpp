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

#include "midievent.h"

//int Event::refCount = 0;
//std::set<Event*> Event::refSet;

namespace bipscript {
namespace midi {

Event::Event(const Event &other) :
    bipscript::Event(other), type(other.type),
    databyte1(other.databyte1),
    databyte2(other.databyte2),
    channel(other.channel) {}

Event::Event(Position &position, int n, int vel, int t, unsigned char ch)
    : bipscript::Event(position), type(t), databyte1(n), databyte2(vel), channel(ch)  {}


uint8_t Event::dataSize() {
    switch(type) {
    case 0xC0:
        return 1;
    }
    return 2;
}

void Event::pack(void *space) {
    unsigned char *buffer = (unsigned char*)space;
    buffer[0] = this->type + this->channel;
    buffer[1] = this->databyte1;
    if(dataSize() == 2) {
        buffer[2] = this->databyte2;
    }
}

void Event::unpack(const uint8_t *buffer, size_t size)
{
    this->type = buffer[0] & 0xf0;
    this->channel = buffer[0] & 0x0f;
    if(size > 1) {
        this->databyte1 = buffer[1];
    }
    if(size > 2) {
        this->databyte2 = buffer[2];
    }
}

bool Event::matches(int type) { // TODO: what about channels
    return type == this->type;
}

bool Event::matches(int type, int note, int low, int high) {
    return type == this->type
            && note == this->databyte1;
//            && low <= this->velocity
//            && high >= this->velocity;
}

std::ostream& operator<< (std::ostream &out, Event &evt)
{
    switch(evt.type) {
    case 0x80:
        out << "NOTE OFF ";
        break;
    case 0x90:
        out << "NOTE ON ";
        break;
    case 0xB0:
        out << "CONTROL ";
        break;
    default:
        out << "UNK[" << (int)evt.type << "] ";
        break;
    }
    out << (unsigned int)evt.databyte1 << ":";
    out << (unsigned int)evt.databyte2 << " ";
    out << (Position&)evt;
    out << " (" << evt.getFrameOffset() << ")";
    return out;
}

}}