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
#ifndef EVENTLIST_H
#define EVENTLIST_H

#include "event.h"

class EventList : public ListImpl
{
public:
    void insert(Event *elem)
    {
        if(!first) {
            first = last = elem;
        }
        else if(*(Event*)last <= *elem) {
            last->next = elem;
            last = elem;
        } else if(*elem < *(Event*)first) {
            elem->next = first;
            first = elem;
        } else {
            Event *e = (Event*)first;
            while(*(Event*)(e->next) <= *elem) {
                e = (Event*)e->next;
            }
            elem->next = e->next;
            e->next = elem;
        }
    }
    void clear() {
        first = 0;
    }
    Event *getFirst() {
        return (Event*)ListImpl::getFirst();
    }
    Event *pop() {
        return (Event*)ListImpl::pop();
    }
};

#endif // EVENTLIST_H
