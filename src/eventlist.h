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

#include "listable.h"

namespace bipscript {

template <class T> class EventList : public List<Listable>
{
public:
    void insert(T *elem)
    {
        if(!first) {
            first = last = elem;
        }
        else if(*(T*)last <= *elem) {
            last->next = elem;
            last = elem;
        } else if(*elem < *(T*)first) {
            elem->next = first;
            first = elem;
        } else {
            T *e = (T*)first;
            while(*(T*)(e->next) <= *elem) {
                e = (T*)e->next;
            }
            elem->next = e->next;
            e->next = elem;
        }
    }
    void clear() {
        first = 0;
    }
    T *getFirst() {
        return (T*)List::getFirst();
    }
    T *getNext(T *obj) {
        return (T*)List::getNext(obj);
    }
    T *pop() {
        return (T*)List::pop();
    }
};

}

#endif // EVENTLIST_H
