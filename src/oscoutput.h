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

class OscOutput : public Listable
{
    pthread_t thread;
    lo_address loAddress;
    std::atomic<bool> repositionNeeded;
    std::atomic<bool> cancelled;
    EventBuffer eventBuffer;
public:
    OscOutput(const char *host, int port);
    void schedule(OscMessage &message, int bar);
    void run();
    void reset();
    void reposition() { repositionNeeded.store(true); }
    bool repositionComplete() { return !repositionNeeded.load(); }
    void cancel() { cancelled.store(true); }
};

class OscOutputFactory : public ObjectCache
{
    std::map<std::string, OscOutput*> instanceMap;
    std::set<OscOutput*> activeScriptObjects;
    QueueList<OscOutput> activeProcessObjects;
    boost::lockfree::spsc_queue<OscOutput*> deletedObjects;
    OscOutputFactory() : activeProcessObjects(16), deletedObjects(16) {}
    OscOutput *createObject(std::string key);
public:
    static OscOutputFactory &instance() {
        static OscOutputFactory instance;
        return instance;
    }
    void process(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time) {}
    bool scriptComplete();
    void reposition();
    bool repositionComplete();
    OscOutput *getOscOutput(const char *host, int port);
    void shutdown();
};

#endif // OSCOUTPUT_H
