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
#ifndef LISTABLE_H
#define LISTABLE_H

#include <boost/lockfree/spsc_queue.hpp>

namespace bipscript {

class Listable
{
public:
    Listable *next;
    Listable() : next(0) {}
    virtual ~Listable() {}
};

template <class T> class List
{
protected:
    T *first;
    T *last;
public:
    List() : first(0) {}
    void add(T *elem) {
        if(!first) {
            last = elem;
        }
        elem->next = first;
        first = elem;
    }
    void addAll(List &list) {
        if(!first) {
            first = list.first;
            last = list.last;
        } else {
            last->next = list.first;
            last = list.last;
        }
    }
    T *getFirst() {
        return first;
    }
    T *getNext(T *elem) {
        return static_cast<T*>(elem->next);
    }
    T *pop() {
        return first = static_cast<T*>(first->next);
    }
    T *remove(T *elem) {
        if(first == elem) {
            first = static_cast<T*>(first->next);
            return first;
        } else {
            Listable *l = first;
            while(l->next != elem) {
                l = l->next;
            }
            l->next = l->next->next;
            return static_cast<T*>(l->next);
        }
    }
    void clear() {
        first = 0;
    }
};


template <class T> class QueueList
{
    List<T> list;
    boost::lockfree::spsc_queue<T*> queue;
public:
    QueueList(std::size_t size) : queue(size) {}
    void add(T *elem) {
        while(!queue.push(elem));
    }
    T *getFirst() {
        T *fresh;
        while(queue.pop(fresh)) {
            list.add(fresh);
        }
        return list.getFirst();
    }
    T *getNext(T *elem) {
        return list.getNext(elem);
    }
    T *pop() {
        return list.pop();
    }
    T *remove(T *elem) {
        return list.remove(elem);
    }
    void clear() {
        T *fresh;
        while(queue.pop(fresh)) {}
        list.clear();
    }
};

}

#endif // LISTABLE_H
