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
#ifndef EVENTCLOSURE_H
#define EVENTCLOSURE_H

#include "scripttypes.h"
#include "listable.h"
#include "memorypool.h"
#include "objectcollector.h"

namespace bipscript {
/**
  *
  * represents a closure created from the process thread
  *
*/
class EventClosure : public ScriptFunctionClosure, public Listable
{  
public:
  EventClosure(ScriptFunction &function) :
      ScriptFunctionClosure(function) {}
  void* operator new(size_t size) {
    return MemoryPool::processPool().malloc(size);
  }
  void operator delete(void *p) {
    MemoryPool::processPool().free(p);
  }
  void recycle() {
    ObjectCollector::processCollector().recycle(this);
  }
};

}

#endif // EVENTCLOSURE_H
