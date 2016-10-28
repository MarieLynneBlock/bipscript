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

#include "timesignature.h"
#include "processor.h"

class TransportMaster;

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

    // processors
    QueueList<Processor> activeProcessors;
    boost::lockfree::spsc_queue<Processor*> deletedProcessors; // TODO: also a QueueList?

    // private methods
    bool reposition(uint16_t attempt);

    // singleton
    AudioEngine() : client(0), activeProcessors(128), deletedProcessors(16) {}
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
    void setBufferSize(jack_nframes_t size);
    void addProcessor(Processor *obj) {
        activeProcessors.add(obj);
    }
    void removeProcessor(Processor *obj) {
        while(!deletedProcessors.push(obj));
    }
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
    void transportRelocate(jack_nframes_t frame) {
        jack_transport_locate(client, frame);
    }
    TransportMaster *getTransportMaster(double bpm, float beatsPerBar, float beatUnit);
    void releaseTransportMaster();
};

#endif // AUDIOENGINE_H
