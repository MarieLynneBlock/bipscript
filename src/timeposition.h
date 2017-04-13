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
#ifndef TIMEPOSITION_H
#define TIMEPOSITION_H

#include "position.h"

namespace bipscript {
namespace transport {

class TimePosition : public Position
{
    jack_time_t time;
public:
    TimePosition(unsigned int bar, unsigned int position, unsigned int division)
        : Position(bar, position, division), time(0) {}
    TimePosition(jack_position_t &pos) :
        time(pos.usecs) {
        if(pos.valid) {
            setBar(pos.bar);
            setPosition(pos.tick + (pos.ticks_per_beat * (pos.beat - 1)));
            setDivision(pos.ticks_per_beat * pos.beats_per_bar);
        }
    }
    TimePosition(jack_position_t &pos, long frameOffset) :
        time(pos.usecs) {
        if(pos.valid) {
            setBar(pos.bar);
            // TODO: include frameOffset
            setPosition(pos.tick + (pos.ticks_per_beat * (pos.beat - 1)));
            setDivision(pos.ticks_per_beat * pos.beats_per_bar);
        }
    }
    jack_time_t getTime() { return time; }
};

}}

#endif // TIMEPOSITION_H
