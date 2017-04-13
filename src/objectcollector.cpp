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

#include "objectcollector.h"

namespace bipscript {

// called by process thread to recycle a single event
void ObjectCollector::recycle(Listable *evt) {
    // try to push to queue
    if(!objectQueue.push(evt)) {
        // q is full, add to waiting list
        waitingList.add(evt);
    }
}

// called by process thread to recycle a list of events
void ObjectCollector::recycleAll(List<Listable> &list) {
    // append rest to waiting list
    waitingList.addAll(list);
    // push to queue as many as we can
    Listable *next = waitingList.getFirst();
    while(next && objectQueue.push(next)) {
        next = waitingList.pop();
    }
}

// called by process thread to shrink the waiting list
void ObjectCollector::update() {
    Listable *waiting = waitingList.getFirst();
    while(waiting && objectQueue.push(waiting)) {
        waiting = waitingList.pop();
    }
}

// called by script thread to delete available events
void ObjectCollector::free() {
    Listable *event;
    while (objectQueue.pop(event)) {
        delete event;
    }
}

}