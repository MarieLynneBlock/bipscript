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

#include "position.h"
#include <stdexcept>

#include <iostream>

namespace bipscript {

Duration::Duration(unsigned int whole, unsigned int position, unsigned int division)
    : whole(whole), position(position), division(division) {
    if(division == 0) {
        //std::cerr << "division = 0 at " << bar << ":" << position <<"/0" << std::endl;
        throw std::logic_error("division cannot be zero!");
    }
    normalize();
}

void Duration::normalize()
{
    // over unity
    if(position >= division) {
            whole += position / division;
            position = position % division;
    }
    // reduce
    if(division > MAX_DENOMINATOR) {
        for(int factor = 7; factor > 1; factor--) {
            while(position % factor == 0 && division % factor == 0) {
                position /= factor;
                division /= factor;
            }
        }
    }
	// TODO: round if still  > MAX_DENOMINATOR
}

bool Duration::operator<(Duration &other)
{
    if(this->whole < other.whole) {
        return true;
    }
    if(this->whole == other.whole) {
        return (this->position * other.division) < (other.position * this->division);
    }
    return false;
}

bool Duration::operator<=(Duration &other)
{
    if(this->whole < other.whole) {
        return true;
    }
    if(this->whole == other.whole) {
        return (this->position * other.division) <= (other.position * this->division);
    }
    return false;
}

const Duration Duration::operator-(const Duration &other)
{
    Duration result = *this;
    if(result.division == other.division) {
        while(result.whole > other.whole) {
            result.whole--;
            result.position += division;
        }
        result.whole = 0;
        result.position -= other.position;
        // result.normalize();
    } else {
        throw std::logic_error("cannot subtract different divisors (not implemented)");
    }
    return result;
}


std::ostream& operator<< (std::ostream &out, Duration &duration)
{
    out << "d" << duration.whole << "+" <<
               duration.position << "/" << duration.division;
    return out;
}


// ----------- Position


Position::Position(unsigned int bar, unsigned int position, unsigned int division) :
    Duration(bar, position, division)
{
    if(bar == 0) {
        //std::cerr << "position = 0 at " << bar << ":0/" << division << std::endl;
        throw std::logic_error("there is no zero bar");
    }
}


long Position::calculateFrameOffset(jack_position_t &pos)
{
    // calculate event tick from position/division
    unsigned int evtTick = (this->position * pos.ticks_per_beat * pos.beats_per_bar) / this->division;
    // calculate event tick offset from pos
    long tickOffset = ((int)(this->getBar() - pos.bar) * pos.beats_per_bar + 1 - pos.beat) * pos.ticks_per_beat + evtTick - pos.tick;
    // ticks to frames
    return tickOffset * pos.frame_rate * 60 / (pos.ticks_per_beat * pos.beats_per_minute);
}

Position &Position::operator+=(const Duration &duration)
{
    // std::cout << "adding " << duration << " to " << *this;
    this->whole += duration.getBar();
    if(this->division == duration.getDivision()) {
        this->position += duration.getPosition();
    } else {
        this->position = position * duration.getDivision() + duration.getPosition() * division;
        this->division *= duration.getDivision();
    }
    // std::cout << " yields " << *this << ", " << std::endl;
    normalize();
    return *this;
}

std::ostream& operator<< (std::ostream &out, Position &pos)
{
    out << "@" << pos.getBar() << "+" <<
               pos.position << "/" << pos.division;
    return out;
}

}