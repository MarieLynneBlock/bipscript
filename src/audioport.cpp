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
#include "audioengine.h"

#include <cstring>

AudioInputPort *AudioInputPortCache::getAudioInputPort(const char *name, const char *connection)
{
    // see if port already exists in map
    int key = std::hash<std::string>()(name);
    AudioInputPort *port = findObject(key);
    if (!port) {
        // create new jack port
        jack_port_t *jackPort = AudioEngine::instance().registerAudioInputPort(name);
        if(!jackPort) {
            throw "Failed to register port ";
        }
        // add to map
        port = new AudioInputPort(jackPort);
        registerObject(key, port);
        // auto connect output port
    }
    if (connection) {
        port->systemConnect(connection);
    }
    return port;
}

AudioOutputPort::~AudioOutputPort()
{
    AudioEngine::instance().unregisterPort(port);
}

void AudioOutputPort::doProcess(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time) {
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

AudioOutputPort *AudioOutputPortCache::getAudioOutputPort(const char* portName, const char* connection)
{
    int key = std::hash<std::string>()(portName);
    // see if port already exists in map
    AudioOutputPort *port = findObject(key);
    if(!port) {
        // create system port
        jack_port_t *jackPort = AudioEngine::instance().registerAudioOutputPort(portName);
        if(!jackPort) {
            throw std::logic_error(std::string("Failed to register port ") + portName);
        }
        port = new AudioOutputPort(jackPort);
		registerObject(key, port);
    }
    if(connection) {
        port->systemConnect(connection);
    }
    return port;
}

AudioStereoOutput::AudioStereoOutput(std::string name, const char *connectLeft, const char *connectRight)
{
    portLeft = AudioOutputPortCache::instance().getAudioOutputPort((name + "L").c_str(), connectLeft);
    portRight = AudioOutputPortCache::instance().getAudioOutputPort((name + "R").c_str(), connectRight);
}

void AudioStereoOutput::connect(AudioSource &source) {
    if(source.getAudioOutputCount() == 1) {
        portLeft->connect(source.getAudioConnection(0));
        portRight->connect(source.getAudioConnection(0));
    } else {
        portLeft->connect(source.getAudioConnection(0));
        portRight->connect(source.getAudioConnection(1));
    }
}

void AudioStereoInput::doProcess(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time)
{
    portLeft->process(rolling, pos, nframes, time);
    portRight->process(rolling, pos, nframes, time);
}

void AudioStereoInput::reset(std::string name, const char *connectLeft, const char *connectRight)
{
    portLeft = AudioInputPortCache::instance().getAudioInputPort((name + "L").c_str(), connectLeft);
    portRight = AudioInputPortCache::instance().getAudioInputPort((name + "R").c_str(), connectRight);
}

AudioConnection *AudioStereoInput::getAudioConnection(unsigned int index)
{
    if(index == 0) {
        return portLeft->getAudioConnection(0);
    } else if(index == 1) {
        return portRight->getAudioConnection(0);
    } else {
        throw std::logic_error("stereo port has only two connections");
    }
}

AudioStereoInput *AudioStereoInputCache::getAudioStereoInput(const char *name, const char *connectLeft, const char *connectRight)
{
    int key = std::hash<std::string>()(name);
    AudioStereoInput *input = findObject(key);
    if(!input) {
        input = new AudioStereoInput(name, connectLeft, connectRight);
        registerObject(key, input);
    } else {
        input->reset(name, connectLeft, connectRight);
    }
    return input;
}
