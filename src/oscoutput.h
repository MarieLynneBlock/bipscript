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
#ifndef OSCOUTPUT_H
#define OSCOUTPUT_H

#include "oscmessage.h"
#include "eventbuffer.h"
#include "objectcache.h"

#include <pthread.h>
#include <atomic>
#include <lo/lo.h>

namespace bipscript {
namespace osc {

class OscOutput : public Processor
{
    pthread_t thread;
    lo_address loAddress;
    std::atomic<bool> repositionNeeded;
    std::atomic<bool> cancelled;
    EventBuffer<OscEvent> eventBuffer;
public:
    OscOutput(const char *host, int port);
    void schedule(OscMessage &message, int bar, int position, int division);
    void schedule(OscMessage &message, int bar, int position) {
        schedule(message, bar, position, 4); // TODO: current time signature?
    }
    void schedule(OscMessage &message, int bar) {
        schedule(message, bar, 0);
    }
    void send(OscMessage &message) {
        Position pos;
        eventBuffer.addEvent(new OscEvent(pos, message));
    }
    void run();
    void reset();
    void doProcess(bool, jack_position_t&, jack_nframes_t, jack_nframes_t) {}
    void reposition() { repositionNeeded.store(true); }
    bool repositionComplete() { return !repositionNeeded.load(); }
    void cancel() { cancelled.store(true); }
};

class OscOutputFactory : public ProcessorCache<OscOutput>
{
public:
    static OscOutputFactory &instance() {
        static OscOutputFactory instance;
        return instance;
    }
    OscOutput *getOscOutput(const char *host, int port);
    void shutdown() { removeAll(); }
    void removeObject(OscOutput *obj) { obj->cancel(); }
};

}}

#endif // OSCOUTPUT_H
