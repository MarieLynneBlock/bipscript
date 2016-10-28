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
#ifndef AUDIOOUTPUT_H
#define AUDIOOUTPUT_H

#include <jack/jack.h>
#include <atomic>
#include <map>

#include "listable.h"
#include "audioconnection.h"
#include "objectcache.h"
#include "audioengine.h"

class AudioInputPort : public AudioSource
{
    jack_port_t* port;
    AudioConnection connection;
public:
    AudioInputPort(jack_port_t *jackPort)
        : port(jackPort), connection(this, false) { }
    void systemConnect(const char *name) {
        AudioEngine::instance().connectPort(name, port);
    }
    // Source interface
    bool connectsTo(Source *) { return false; }
    void doProcess(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time) {
        connection.setBuffer((float*)jack_port_get_buffer (port, nframes));
    }
    void reposition() {}
    // AudioSource interface
    unsigned int getAudioOutputCount() { return 1; }
    AudioConnection *getAudioConnection(unsigned int) {
        return &connection;
    }
};

class AudioInputPortCache : public ProcessorCache<AudioInputPort>
{
public:
    static AudioInputPortCache &instance() {
        static AudioInputPortCache instance;
        return instance;
    }
    AudioInputPort *getAudioInputPort(const char* name, const char* connectTo);
    AudioInputPort *getAudioInputPort(const char* name) {
        return getAudioInputPort(name, 0);
    }
};

// represents a system output port
class AudioOutputPort : public Processor
{
    jack_port_t* port;
    std::atomic<AudioConnection *> audioInput;
    std::string connected;
public:
    AudioOutputPort(jack_port_t *jackPort) : port(jackPort), audioInput(0) { }
    ~AudioOutputPort();
    jack_port_t* getJackPort() { return port; }
    void doProcess(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time);
    void connect(AudioSource &source) {
        connect(source.getAudioConnection(0));
    }
    void connect(AudioConnection &connection) {
        this->audioInput.store(&connection);
    }
    void connect(AudioConnection *connection) {
        this->audioInput.store(connection);
    }
    void reposition() {}
    void systemConnect(const char *connection);
};

class AudioOutputPortCache : public ProcessorCache<AudioOutputPort>
{
public:
    static AudioOutputPortCache &instance() {
        static AudioOutputPortCache instance;
        return instance;
    }
    AudioOutputPort *getAudioOutputPort(const char *portName, const char *connectTo);
    AudioOutputPort *getAudioOutputPort(const char *portName) {
        return getAudioOutputPort(portName, 0);
    }
};

/**
 * represents a stereo system output
 *
 * does not need a cache, lifecycle is simple script object
 */
class AudioStereoOutput
{
    AudioOutputPort *portLeft;
    AudioOutputPort *portRight;
public:
    void connect(AudioSource &source);
    void connect(AudioConnection &connection) {
        portLeft->connect(&connection);
    }
    AudioStereoOutput(std::string name, const char *connectLeft, const char *connectRight);
};

class AudioStereoInput : public AudioSource
{
    AudioInputPort *portLeft;
    AudioInputPort *portRight;
public:
    AudioStereoInput(const char *name, const char *connectLeft, const char *connectRight) {
        reset(name, connectLeft, connectRight);
    }
    void reset(std::string name, const char *connectLeft, const char *connectRight);
    // Source interface
    bool connectsTo(Source *) { return false; }
    void doProcess(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time);
    void reposition() {}
    // AudioSource interface
    unsigned int getAudioOutputCount() { return 2; }
    AudioConnection *getAudioConnection(unsigned int index);
};

class AudioStereoInputCache : public ProcessorCache<AudioStereoInput>
{
public:
    static AudioStereoInputCache &instance() {
        static AudioStereoInputCache instance;
        return instance;
    }
    AudioStereoInput *getAudioStereoInput(const char* name, const char *connectLeft, const char *connectRight);
};

#endif // AUDIOOUTPUT_H
