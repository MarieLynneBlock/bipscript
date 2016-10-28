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
#ifndef MIDIPORT_H
#define MIDIPORT_H

#include "eventbuffer.h"
#include "midiconnection.h"
#include "midisink.h"
#include "objectcache.h"

#include <jack/jack.h>
#include <jack/midiport.h>

#include <map>
#include <set>

class MidiInputConnection : public MidiConnection
{
    jack_port_t* jackPort;
    void *buffer;
    MidiEvent lastEvent;
public:
    MidiInputConnection(MidiSource *source, jack_port_t *jackPort)
        : MidiConnection(source), jackPort(jackPort) {}
    void process(jack_nframes_t nframes) {
        buffer = jack_port_get_buffer(jackPort, nframes);
    }
    uint32_t getEventCount() {
        return jack_midi_get_event_count(buffer);
    }
    void systemConnect(const char *name) {
        AudioEngine::instance().connectPort(name, jackPort);
    }
    MidiEvent *getEvent(uint32_t i);
};

class MidiInputPort : public MidiSource
{
    MidiInputConnection connection;
public:
    MidiInputPort(jack_port_t *jackPort)
        : connection(this, jackPort) {}
    void systemConnect(const char *name) {
        connection.systemConnect(name);
    }
    MidiConnection &getMidiConnection(unsigned int) {
        return connection;
    }
    unsigned int getMidiOutputCount() { return 1; }
    // Source interface
    bool connectsTo(Source *) { return false; }
    void doProcess(bool, jack_position_t&, jack_nframes_t nframes, jack_nframes_t) {
        connection.process(nframes);
    }
    void reposition() {}
};

class MidiInputPortCache : public ProcessorCache<MidiInputPort>
{
public:
    static MidiInputPortCache &instance() {
        static MidiInputPortCache instance;
        return instance;
    }
    MidiInputPort *getMidiInputPort(const char*  name, const char* connectTo);
    MidiInputPort *getMidiInputPort(const char*  name) {
        return getMidiInputPort(name, 0);
    }
};

class MidiOutputPort : public Processor, public MidiSink
{
    jack_port_t* jackPort;
    EventBuffer buffer;
    std::string connected;
public:
    MidiOutputPort(jack_port_t *jackPort) : jackPort(jackPort) {}
    ~MidiOutputPort();
    void systemConnect(const char *connection);
    void addMidiEvent(MidiEvent* evt)  { buffer.addEvent(evt);}
    // Processor interface
    void doProcess(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time);
    void reposition() { buffer.recycleRemaining(); }
};

class MidiOutputPortCache : public ProcessorCache<MidiOutputPort>
{
public:
    static MidiOutputPortCache &instance() {
        static MidiOutputPortCache instance;
        return instance;
    }
    MidiOutputPort *getMidiOutputPort(const char *portName, const char *connectTo);
    MidiOutputPort *getMidiOutputPort(const char *portName) {
        return getMidiOutputPort(portName, 0);
    }
};

#endif // MIDIPORT_H
