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

class AudioInputPort : public AudioSource
{
    jack_port_t* port;
    AudioConnection connection;
public:
    AudioInputPort(jack_port_t *jackPort)
        : port(jackPort), connection(this, false) { }
    // Source interface
    bool connectsTo(Source *) { return false; }
    void process(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time);
    // AudioSource interface
    unsigned int getAudioOutputCount() { return 1; }
    AudioConnection &getAudioConnection(unsigned int) {
        return connection;
    }
};

class AudioInputPortCache // TODO: extend ObjectCache
{
    std::map<std::string, AudioInputPort *> audioInputPortMap;
public:
    static AudioInputPortCache &instance() {
        static AudioInputPortCache instance;
        return instance;
    }
    AudioInputPort *getAudioInputPort(const char*  name);
    AudioInputPort *getAudioInputPort(const char*  name, const char* connectTo);
};

// represents a system output port
class AudioOutputPort : public Listable
{
    jack_port_t* port;
    std::atomic<AudioConnection *> audioInput;
    std::string connected;
public:
    AudioOutputPort(jack_port_t *jackPort) : port(jackPort), audioInput(0) { }
    ~AudioOutputPort();
    jack_port_t* getJackPort() { return port; }
    void processAll(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time);
    void connect(AudioConnection &connection) {
        this->audioInput.store(&connection);
    }
    void reposition() {}
    void systemConnect(const char *connection);
};

class AudioOutputPortCache : public ProcessorCache<std::string, AudioOutputPort>
{
    AudioOutputPort *createObject(std::string key);
public:
    static AudioOutputPortCache &instance() {
        static AudioOutputPortCache instance;
        return instance;
    }
    AudioOutputPort *getAudioOutputPort(const char *portName, const char *connectTo);
};

#endif // AUDIOOUTPUT_H
