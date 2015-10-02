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
#ifndef EVENTBUFFER_H
#define EVENTBUFFER_H

#include "event.h"
#include "eventlist.h"

#include <boost/lockfree/spsc_queue.hpp>

class EventBuffer
{
    boost::lockfree::spsc_queue<Event*> eventQueue; // script thread -> process thread
    EventList sortedEvents; // local to process thread
public:
    EventBuffer() : eventQueue(2048) {}
    void addEvent(Event* evt);
    void update();
    Event *getNextEvent(bool rolling, jack_position_t &pos, jack_nframes_t nframes);
    void recycleRemaining();
};

#endif // EVENTBUFFER_H
