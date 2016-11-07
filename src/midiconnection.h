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
#ifndef MIDICONNECTION_H
#define MIDICONNECTION_H

#include <atomic>
#include <jack/types.h>
#include "source.h"
#include "midievent.h"

class MidiConnection;

class MidiSource : virtual public Source
{
public:
    virtual unsigned int getMidiOutputCount() = 0;
    virtual MidiConnection *getMidiConnection(unsigned int index) = 0;
};

class MidiConnection {
    MidiSource *source;
public:
    MidiConnection(MidiSource *source) : source(source) {}
    void process(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time) {
        source->process(rolling, pos, nframes, time);
    }
    virtual uint32_t getEventCount() = 0;
    virtual MidiEvent *getEvent(uint32_t i) = 0;
    bool connectsTo(Source *src) {
        return source == src || source->connectsTo(src);
    }
};

class MidiConnector {
    std::atomic<MidiConnection *> connection;
public:
    MidiConnector() : connection(0) {}
    void setConnection(MidiConnection *conn, Source *source) {
        if(conn->connectsTo(source)) {
            throw std::logic_error("Cannot connect infinite loop");
        }
        connection.store(conn);
    }
    MidiConnection *getConnection() {
        return connection.load();
    }
};

#endif // MIDICONNECTION_H
