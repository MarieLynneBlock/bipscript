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
#ifndef MIXER_H
#define MIXER_H

#include "audioconnection.h"
#include "midiconnection.h"
#include "listable.h"
#include "eventbuffer.h"
#include "objectcache.h"

#include <iostream>
#include <boost/lockfree/spsc_queue.hpp>
#include <map>

using boost::lockfree::spsc_queue;
using std::map;

class MixerControlConnection;
class ScriptArray;

struct MixerControlMapping : public Listable
{
    MixerControlConnection *connection;
    unsigned int cc;
    unsigned int input;
    unsigned int output;
    MixerControlMapping(MixerControlConnection *connection, unsigned int cc, unsigned int input, unsigned int output) :
        connection(connection), cc(cc), input(input), output(output) {
    }
};

class MixerControlConnection : public Listable
{
    List<MixerControlMapping> mappings;
    MidiConnection *connection;
    u_int32_t eventCount;
    u_int32_t eventIndex;
public:
    MixerControlConnection(MidiConnection *connection) :
        connection(connection) {}
    void addMapping(MixerControlMapping *mapping) {
        mappings.add(mapping);
    }
    void process(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time);
    void updateGains(jack_nframes_t frame, float **gains);
    void reposition();
};

class MixerGainEvent : public Event {
    uint32_t input;
    uint32_t output;
    float value;
public:
    MixerGainEvent(uint32_t input, uint32_t output, float value,
                   uint32_t bar, uint32_t position, uint32_t division) :
        Event(bar, position, division), input(input), output(output), value(value) {}
    uint32_t getInput() {
        return input;
    }
    uint32_t getOutput() {
        return output;
    }
    float getValue() {
        return value;
    }
};


class Mixer : public AudioSource, public Listable
{
    float **gain;   // TODO: threadsafe?
    jack_nframes_t processedUntil;
    // audio inputs
    std::atomic<unsigned int> connectedInputs;
    const unsigned int audioInputCount;
    AudioConnector *audioInput;
    // audio outputs
    const unsigned int audioOutputCount;
    AudioConnection **audioOutput;
    // control connections
    map<MidiConnection*,MixerControlConnection*> controlConnectionMap;
    spsc_queue<MixerControlMapping*> newControlMappingsQueue;
    QueueList<MixerControlConnection> controlConnections;
    EventBuffer gainEventBuffer;
public:
    Mixer(unsigned int inputs, const unsigned int outputs);
    ~Mixer();
    void connect(AudioSource &source, ScriptArray &gains);
    void connect(AudioSource &source, float gain);
    void connect(AudioSource &source) {
        connect(source, 1.0);
    }
    void addGainController(MidiSource &source, unsigned int cc, unsigned int input, unsigned int output);
    void scheduleGain(uint32_t input, uint32_t output, float gain, uint32_t bar, uint32_t position, uint32_t division);
    void restore();
    void processAll(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time);
    // Source interface
    bool connectsTo(Source *source);
    void process(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time);
    void reposition();
    // AudioSource interface
    unsigned int getAudioOutputCount() { return audioOutputCount; }
    AudioConnection *getAudioConnection(unsigned int index) { return audioOutput[index]; }
private:
    void validateInputChannel(uint32_t input);
    void validateOutputChannel(uint32_t output);
};

class MixerCache : public ProcessorCache<Mixer>
{
    std::map<int, int> instanceCount;
    void scriptReset() {
        instanceCount.clear();
    }
public:
    static MixerCache &instance() {
        static MixerCache instance;
        return instance;
    }
    Mixer *getMixer(uint16_t inputs, uint16_t outputs);
};

#endif // MIXER_H
