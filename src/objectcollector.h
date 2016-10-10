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
#ifndef OBJECTCOLLECTOR_H
#define OBJECTCOLLECTOR_H

#include "eventlist.h"
#include <boost/lockfree/queue.hpp>

class ObjectCollector
{
    boost::lockfree::queue<Listable*> objectQueue; // script thread -> collector thread
    ListImpl waitingList;
    // singleton
    ObjectCollector() : objectQueue(4096) {}
    ObjectCollector(ObjectCollector const&);
    void operator=(ObjectCollector const&);
public:
    static ObjectCollector &scriptCollector() {
        static ObjectCollector instance;
        return instance;
    }
    static ObjectCollector &processCollector() {
        static ObjectCollector instance;
        return instance;
    }
    void recycle(Listable *collectable);
    void recycleAll(EventList &list);
    void update();
    void free();
};

#endif // OBJECTCOLLECTOR_H
