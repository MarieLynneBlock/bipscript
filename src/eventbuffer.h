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

#include "eventlist.h"
#include "objectcollector.h"

#include <jack/types.h>
#include <boost/lockfree/spsc_queue.hpp>

#define UPDATE_MAX_EVENTS 32

template <class T> class EventBuffer
{
    boost::lockfree::spsc_queue<T*> eventQueue; // script thread -> process thread
    EventList<T> sortedEvents; // local to process thread
public:
    EventBuffer() : eventQueue(2048) {}
    void addEvent(T* evt);
    void update();
    T *getNextEvent(bool rolling, jack_position_t &pos, jack_nframes_t nframes);
    void recycleRemaining();
};

// runs in script thread
template <class T>
void EventBuffer<T>::addEvent(T *evt)  {
    while(!eventQueue.push(evt)); // maybe wait a bit?
}

// process thread
template <class T>
void EventBuffer<T>::update()
{
    T *freshEvent;
    int counter = 0;
    while (counter < UPDATE_MAX_EVENTS && eventQueue.pop(freshEvent)) {
        sortedEvents.insert(freshEvent);
        counter++;
    }
}

// process thread
template <class T>
T *EventBuffer<T>::getNextEvent(bool rolling, jack_position_t &pos, jack_nframes_t nframes)
{
    update();
    if(rolling) {
        T *first = sortedEvents.getFirst();
        // drop events that have already passed
        while(first && first->updateFrameOffset(pos) < -256) { // TODO: grace period depends on framerate
            T *late = first; // grab reference, cannot delete before pop()
            first = sortedEvents.pop();
            ObjectCollector::scriptCollector().recycle(late);
        }
        // return the top event if it fits in this buffer
        if(first && first->getFrameOffset() < nframes) {
            sortedEvents.pop();
            return first;
        }
    }
    // no events for this position
    return 0;
}

template <class T>
void EventBuffer<T>::recycleRemaining()
{
    ObjectCollector &collector = ObjectCollector::scriptCollector();
    // clear incoming queue
    T *nextEvent;
    while (eventQueue.pop(nextEvent)) {
        collector.recycle(nextEvent);
    }
    // clear existing events
    collector.recycleAll(sortedEvents);
    sortedEvents.clear();
}

#endif // EVENTBUFFER_H
