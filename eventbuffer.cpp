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

#include "eventbuffer.h"
#include "objectcollector.h"

// runs in script thread
void EventBuffer::addEvent(Event *evt)  {
    while(!eventQueue.push(evt)); // maybe wait a bit?
}

void EventBuffer::update()
{
    Event *freshEvent;
    while (eventQueue.pop(freshEvent)) {
        sortedEvents.insert(freshEvent);
    }
}

// process thread
Event *EventBuffer::getNextEvent(bool rolling, jack_position_t &pos, jack_nframes_t nframes)
{
    update();
    if(rolling) {
        Event *first = sortedEvents.getFirst();
        // drop events that have already passed
        while(first && first->updateFrameOffset(pos) < -256) { // TODO: grace period depends on framerate
            Event *late = first; // grab reference, cannot delete before pop()
            first = sortedEvents.pop();
            ObjectCollector::instance()->recycle(late);
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

void EventBuffer::recycleRemaining()
{
    ObjectCollector *collector = ObjectCollector::instance();
    // clear incoming queue
    Event *nextEvent;
    while (eventQueue.pop(nextEvent)) {
        collector->recycle(nextEvent);
    }
    // clear existing events
    collector->recycleAll(sortedEvents);
    sortedEvents.clear();
}
