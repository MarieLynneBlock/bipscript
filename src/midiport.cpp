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

#include "midiport.h"
#include "objectcollector.h"
#include "audioengine.h"

namespace bipscript {
namespace midi {

MidiEvent *MidiInputConnection::getEvent(uint32_t i) {
    jack_midi_event_t in_event;
    jack_midi_event_get(&in_event, buffer, i);
    lastEvent.unpack(in_event.buffer, in_event.size);
    lastEvent.setFrameOffset(in_event.time);
    return &lastEvent;
}

MidiInputPort *MidiInputPortCache::getMidiInputPort(const char *name, const char *connectTo)
{
    // see if port already exists in map
    int key = std::hash<std::string>()(name);
    MidiInputPort *port = findObject(key);
    if (!port) {
        // create new jack port
        jack_port_t *jackPort = AudioEngine::instance().registerMidiInputPort(name);
        if(!jackPort) {
            std::string message = "Failed to register midi input port: ";
            throw message + name;
        }
        // add to map
        port = new MidiInputPort(jackPort);
        registerObject(key, port);
    }
    // auto-connect
    if(connectTo) {
        port->systemConnect(connectTo);
    }
    return port;
}

MidiOutputPort::~MidiOutputPort()
{
    AudioEngine::instance().unregisterPort(jackPort);
}

void MidiOutputPort::doProcess(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t) {

    // grab and clear the buffer for this port
    void* port_buf = jack_port_get_buffer(jackPort, nframes);
    jack_midi_clear_buffer(port_buf);
    // schedule events that are waiting in the buffer
    MidiEvent* nextEvent = buffer.getNextEvent(rolling, pos, nframes);
    while(nextEvent) {
        long frame = nextEvent->getFrameOffset();
        size_t size = nextEvent->dataSize() + 1;
        unsigned char* jackEvent = jack_midi_event_reserve(port_buf, frame >= 0 ? frame : 0, size);
        nextEvent->pack(jackEvent);
        ObjectCollector::scriptCollector().recycle(nextEvent);
        nextEvent = buffer.getNextEvent(rolling, pos, nframes);
    }
}

void MidiOutputPort::systemConnect(const char *connection) {
    if(connected != connection) {
        if(connected.length() > 0) {
            AudioEngine::instance().disconnectPort(jackPort, connected.c_str());
        }
        AudioEngine::instance().connectPort(jackPort, connection);
        connected = connection;
    }
}

MidiOutputPort *MidiOutputPortCache::getMidiOutputPort(const char* portName, const char* connection)
{
    int key = std::hash<std::string>()(portName);
    // see if port already exists in map
    MidiOutputPort *port = findObject(key);
    if(!port) {
        // create system port
        jack_port_t *jackPort = AudioEngine::instance().registerMidiOutputPort(portName);
        if(!jackPort) {
            throw std::logic_error(std::string("Failed to register port ") + portName);
        }
        port = new MidiOutputPort(jackPort);
        registerObject(key, port);
    }
    if(connection) {
        port->systemConnect(connection);
    }
    return port;
}

}}