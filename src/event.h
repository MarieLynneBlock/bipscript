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
#ifndef EVENT_H
#define EVENT_H

#include "position.h"
#include "listable.h"

//#include <set> // debuggin

namespace bipscript {

class Event : public Position, public Listable
{
//    static int refCount; // debugging
//    static std::set<Event*> refSet;
    long frameOffset;
public:
    Event(Position &pos) : Position(pos) {} // refCount++; refSet.insert(this); }
    Event(const Event &other) : Position(other) {}
    Event(unsigned int bar, unsigned int position, unsigned int division) :
        Position(bar, position, division) {} // refCount++; refSet.insert(this); }
    long getFrameOffset() const {
        return this->frameOffset;
    }
    void setFrameOffset(long offset) {
        this->frameOffset = offset;
    }
    long updateFrameOffset(jack_position_t &pos) {
        return this->frameOffset = calculateFrameOffset(pos);
    }
    // debug
    /*
    virtual void print() = 0;
    ~Event() {
        int test = --Event::refCount;
        //if(test % 1000 == 0 || test < 500) {
            std::cout << "deleted event: " << test << " remaining" << std::endl;
            std::cout.flush();
        //}
        refSet.erase(this);
    }
    static void printRemainders() {
        for (auto it=refSet.begin(); it!=refSet.end(); ++it) {
           std::cout << "R-> ";
           std::cout << *(*it);
            std::cout << std::endl;
        }
    }
    */
};

}

#endif // EVENT_H
