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
#ifndef BTRANSPORT_H
#define BTRANSPORT_H

#include "position.h"
#include "scripttypes.h"
#include "eventbuffer.h"
#include "event.h"
#include "audioengine.h"

namespace bipscript {
namespace transport {

class AsyncClosure : public Event, public ScriptFunctionClosure {
    int nparams;
protected:
    void addParameters() {
        if(nparams > 1) {
         addInteger(getBar());
        }
        if(nparams > 2) {
         addInteger(getPosition());
        }
        if(nparams > 3) {
         addInteger(getDivision());
        }
    }
public:
    AsyncClosure(ScriptFunction &function, unsigned int bar,
                 unsigned int position, unsigned int division) :
        ScriptFunctionClosure(function), Event(bar, position, division)
    {
        nparams = function.getNumargs();
    }
    void recycle() { delete this; }
};

class Transport : public Processor
{
    EventBuffer<AsyncClosure> eventBuffer;
public:
    Transport() {
        AudioEngine::instance().addProcessor(this);
    }
    static Transport &instance() {
        static Transport instance;
        return instance;
    }
    //static Position &now();
    void schedule(ScriptFunction &function, unsigned int bar) {
        schedule(function, bar, 0);
    }
    void schedule(ScriptFunction &function, unsigned int bar, unsigned int position) {
        schedule(function, bar, position, 4); // TODO: base it on time signature
    }
    void schedule(ScriptFunction &function, unsigned int bar, unsigned int position, unsigned int division);
    void doProcess(bool, jack_position_t&, jack_nframes_t, jack_nframes_t);
    void reposition() { eventBuffer.recycleRemaining(); }
    bool repositionComplete() { return true; }
};

}}

#endif // TRANSPORT_H
