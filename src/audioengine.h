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
#ifndef AUDIOENGINE_H
#define AUDIOENGINE_H

#include <jack/jack.h>

#include "position.h"
#include "objectcache.h"
#include "timesignature.h"

class TransportMaster;

#include <map> // need it?

class AudioEngine
{
    // Jack client + info
    jack_client_t *client;
    jack_nframes_t sampleRate;

    // running location
    jack_nframes_t runningFrame;

    // transport
    TransportMaster *transportMaster;
    unsigned int multiplePeriodRestart;
    TimeSignature currentTimeSignature;

    // private methods
    void reset(bool final);

    // singleton
    AudioEngine() : client(0) {}
    AudioEngine(AudioEngine const&);
    void operator=(AudioEngine const&);
public:
    static AudioEngine &instance() {
        static AudioEngine instance;
        return instance;
    }

    // getters and setters
    jack_nframes_t getSampleRate() {
        return sampleRate;
    }
    bool getPosition(jack_position_t &jack_pos) {
        return jack_transport_query(client, &jack_pos) == JackTransportRolling;
    }
    TimeSignature &getTimeSignature();
    // public methods
    int activate(const char *clientName);
    int process(jack_nframes_t nframes);
    int sync(jack_transport_state_t state, jack_position_t *pos);
    void shutdown();

    // system ports
    jack_port_t *registerMidiInputPort(const char *name) {
        return jack_port_register(client, name, JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);
    }
    jack_port_t *registerMidiOutputPort(const char *name) {
        return jack_port_register(client, name, JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput, 0);
    }
    jack_port_t *registerAudioInputPort(const char *name) {
        return jack_port_register(client, name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
    }
    jack_port_t *registerAudioOutputPort(const char *name) {
        return jack_port_register(client, name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
    }
    void connectPort(jack_port_t *port, const char* connection) {
        jack_connect(client, jack_port_name(port), connection);
    }
    void connectPort(const char *connection, jack_port_t *port) {
        jack_connect(client, connection, jack_port_name(port));
    }
    void disconnectPort(jack_port_t *port, const char* connection) {
        jack_disconnect(client, jack_port_name(port), connection);
    }
    void unregisterPort(jack_port_t *port) {
        jack_port_unregister(client, port);
    }
    // transport
    void transportStop() {
        jack_transport_stop(client);
    }
    void transportStart() {
        jack_transport_start(client);
    }
    TransportMaster *getTransportMaster(double bpm);
    void releaseTransportMaster();
};

#endif // AUDIOENGINE_H
