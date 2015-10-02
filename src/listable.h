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

class Listable
{
    friend class ListImpl;
    friend class EventList;
private:
    Listable *next;
public:
    Listable() : next(0) {}
    virtual ~Listable() {}
};

class ListImpl
{
protected:
    Listable *first;
    Listable *last;
public:
    ListImpl() : first(0) {}
    void add(Listable *elem) {
        if(!first) {
            last = elem;
        }
        elem->next = first;
        first = elem;
    }
    void addAll(ListImpl &list) {
        if(!first) {
            first = list.first;
            last = list.last;
        } else {
            last->next = list.first;
            last = list.last;
        }
    }
    Listable *getFirst() {
        return first;
    }
    Listable *getNext(Listable *elem) {
        return elem->next;
    }
    Listable *pop() {
        return first = first->next;
    }
    Listable *remove(Listable *elem) {
        if(first == elem) {
            first = first->next;
            return first;
        } else {
            Listable *l = first;
            while(l->next != elem) {
                l = l->next;
            }
            l->next = l->next->next;
            return l->next;
        }
    }
    void clear() {
        first = 0;
    }
};

template <class T> class List : public ListImpl
{
public:
    T *getFirst() {
        return (T*)ListImpl::getFirst();
    }
    T *getNext(T *elem) {
        return (T*)ListImpl::getNext(elem);
    }
    T *pop() {
        return (T*)ListImpl::pop();
    }
};

class QueueListImpl
{
    ListImpl list;
    boost::lockfree::spsc_queue<Listable*> queue;
public:
    QueueListImpl(std::size_t size) : queue(size) {}
    void add(Listable *elem) {
        while(!queue.push(elem));
    }
    Listable *getFirst() {
        Listable *fresh;
        while(queue.pop(fresh)) {
            list.add(fresh);
        }
        return list.getFirst();
    }
    Listable *getNext(Listable *elem) {
        return list.getNext(elem);
    }
    Listable *pop() {
        return list.pop();
    }
    Listable *remove(Listable *elem) {
        return list.remove(elem);
    }
    void clear() {
        Listable *fresh;
        while(queue.pop(fresh)) {}
        list.clear();
    }
};

template <class T> class QueueList : public QueueListImpl
{
public:
    QueueList(std::size_t size) : QueueListImpl(size) {}
    T *getFirst() {
        return static_cast<T*>(QueueListImpl::getFirst());
    }
    T *getNext(T *elem) {
        return static_cast<T*>(QueueListImpl::getNext(elem));
    }
    T *pop() {
        return static_cast<T*>(QueueListImpl::pop());
    }
    T *remove(T *elem) {
        return static_cast<T*>(QueueListImpl::remove(elem));
    }
};

#endif // LISTABLE_H
