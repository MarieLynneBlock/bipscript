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
#ifndef EVENTCONNECTION_H
#define EVENTCONNECTION_H

#include <atomic>
#include <jack/types.h>
#include "source.h"
#include "midievent.h"

class EventConnection;

class EventSource : virtual public Source
{
public:
    virtual unsigned int getEventOutputCount() = 0;
    virtual EventConnection &getEventConnection(unsigned int index) = 0;
};

class EventConnection {
    EventSource *source;
public:
    EventConnection(EventSource *source) : source(source) {}
    void process(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time) {
        source->process(rolling, pos, nframes, time);
    }
    virtual uint32_t getEventCount() = 0;
    virtual MidiEvent *getEvent(uint32_t i) = 0;
    bool connectsTo(Source *src) {
        return source == src || source->connectsTo(src);
    }
};

class EventConnector {
    std::atomic<EventConnection *> connection;
public:
    EventConnector() : connection(0) {}
    void setConnection(EventConnection *conn, Source *source) {
        if(conn->connectsTo(source)) {
            throw std::logic_error("Cannot connect infinite loop");
        }
        connection.store(conn);
    }
    EventConnection *getConnection() {
        return connection.load();
    }
};

#endif // EVENTCONNECTION_H
