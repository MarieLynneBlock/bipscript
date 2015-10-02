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

#include "audioport.h"

#include <cstring>
#include <iostream>

#include "audioengine.h"

void AudioInputPort::process(bool, jack_position_t &, jack_nframes_t nframes, jack_nframes_t)
{
    connection.setBuffer((float*)jack_port_get_buffer (port, nframes));
}

AudioInputPort *AudioInputPortCache::getAudioInputPort(const char *name, const char *connection)
{
    // see if port already exists in map
    AudioInputPort *port = audioInputPortMap[name];
    if (!port) {
        // create new jack port
        jack_port_t *jackPort = AudioEngine::instance().registerAudioInputPort(name);
        if(!jackPort) {
            throw "Failed to register port ";
        }
        // add to map
        port = new AudioInputPort(jackPort);
        audioInputPortMap[name] = port;
        // auto connect output port
        if (connection) {
            AudioEngine::instance().connectPort(connection, jackPort);
        }
    }
    return port;
}

AudioOutputPort::~AudioOutputPort()
{
    AudioEngine::instance().unregisterPort(port);
}

void AudioOutputPort::processAll(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time) {
    float *buffer = (float*)jack_port_get_buffer(port, nframes);
    AudioConnection *connection = audioInput.load();
    if(connection) {
        float *audio = connection->getAudio(rolling, pos, nframes, time);
        memcpy(buffer, audio, nframes * sizeof(float)); // TODO: std::copy?
    } else {
        memset(buffer, 0, nframes * sizeof(float));
    }
}

void AudioOutputPort::systemConnect(const char *connection) {
    if(connected != connection) {
        if(connected.length() > 0) {
            AudioEngine::instance().disconnectPort(port, connected.c_str());
        }
        AudioEngine::instance().connectPort(port, connection);
        connected = connection;
    }
}

AudioOutputPort *AudioOutputPortCache::createObject(std::string key)
{
    // create system port
    jack_port_t *jackPort = AudioEngine::instance().registerAudioOutputPort(key.c_str());
    if(!jackPort) {
        throw std::logic_error(std::string("Failed to register port ") + key);
    }
    return new AudioOutputPort(jackPort);
}

AudioOutputPort *AudioOutputPortCache::getAudioOutputPort(const char* portName, const char* connection)
{
    // see if port already exists in map
    AudioOutputPort *port = getObject(portName);
    if(connection) {
        port->systemConnect(connection);
    }
    return port;
}
