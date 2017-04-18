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
#ifndef AUDIOPLUGIN_H
#define AUDIOPLUGIN_H

#include "lv2/lv2plug.in/ns/ext/atom/atom.h"
#include "lv2/lv2plug.in/ns/ext/atom/util.h"
#include "lv2/lv2plug.in/ns/ext/state/state.h"
#include "lv2/lv2plug.in/ns/ext/options/options.h"
#include "lv2/lv2plug.in/ns/ext/worker/worker.h"
#include <lilv-0/lilv/lilv.h>
#include <jack/ringbuffer.h>
#include <semaphore.h>

#include <map>
#include <list>
#include <set>

#include "audioconnection.h"
#include "midiconnection.h"
#include "eventbuffer.h"
#include "midisink.h"
#include "scripttypes.h"
#include "objectcache.h"

namespace bipscript {
namespace lv2 {

typedef std::map<std::string, uint32_t> Lv2UridMap;

class UridMapper
{
    Lv2UridMap uridMap;
public:
    LV2_URID uriToId(const char *uri);
    const char *idToUri(LV2_URID urid);
};

class PathMapper
{
public:
    char *mapAbsolutePath(const char *abstractPath);
    char *mapAbstractPath(const char *absolutePath);
};

class Constants
{
public:
    Constants(LilvWorld *);
    // port types
    LilvNode *lv2AtomPort;
    LilvNode *lv2AudioPort;
    LilvNode *lv2InputPort;
    LilvNode *lv2OutputPort;
    LilvNode *lv2ControlPort;
    // midi
    LilvNode *lv2MidiEvent;
    // atom
    LilvNode *lv2AtomSequence;
    LilvNode *lv2AtomSupports;
    LilvNode *lv2AtomBufferType;
    // presets
    LilvNode *lv2Presets;
    // worker
    LilvNode *lv2WorkerInterface;
    LilvNode *lv2WorkerSchedule;
    // rdf
    LilvNode *lv2RdfsLabel;
};

class AtomTypes
{
public:
    LV2_URID doubleType;
    LV2_URID floatType;
    LV2_URID intType;
    LV2_URID longType;
    LV2_URID stringType;
};

class MidiEvent {
public:
    static uint32_t midiEventTypeId;
    LV2_Atom_Event event;
    uint8_t buffer[3];
    MidiEvent() {
        event.body.type = midiEventTypeId;
        event.body.size = 3;
    }
};

class MidiInput : public Listable
{
    const u_int32_t CAPACITY = 1024;
    LV2_Atom_Sequence *atomSequence;
    midi::MidiConnector eventConnector;
    EventBuffer<midi::Event> eventBuffer;
    bool localRolling;
public:
    MidiInput() : localRolling(false) {
        atomSequence = static_cast<LV2_Atom_Sequence *>(malloc(sizeof(LV2_Atom_Sequence) + CAPACITY));
    }
    LV2_Atom_Sequence *getAtomSequence() {
        return atomSequence;
    }
    void connect(midi::MidiConnection *connection, midi::Source *source) {
        eventConnector.setConnection(connection, source);
    }
    bool connectsTo(AbstractSource *source) {
        midi::MidiConnection *connection = eventConnector.getConnection();
        return connection && connection->getSource()->connectsTo(source);
    }
    void addEvent(midi::Event *evt) {
        eventBuffer.addEvent(evt);
    }
    void reset() {
        eventBuffer.recycleRemaining();
    }
    void process(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time);
    void update() {
        eventBuffer.update();
    }
};

class MidiOutput : public Listable, public midi::MidiConnection
{
    const u_int32_t CAPACITY = 1024;
    LV2_Atom_Sequence *atomSequence;    
    midi::Event event;
public:
    MidiOutput(midi::Source *source) : midi::MidiConnection(source) {
        atomSequence = (LV2_Atom_Sequence *)malloc(sizeof(LV2_Atom_Sequence) + CAPACITY);
        atomSequence->atom.size = CAPACITY;
    }
    LV2_Atom_Sequence *getAtomSequence() {
        return atomSequence;
    }
    void clear() {
        atomSequence->atom.size = CAPACITY;
    }
    // EventConnection interface
    uint32_t getEventCount();
    midi::Event *getEvent(uint32_t i);
};

class ControlPort {
public:
    ControlPort() : value(0) {}
    float value;
    float dfault;
    float minimum;
    float maximum;
};

class ControlEvent : public Event {
    ControlPort *port;
    float value;
public:
    ControlEvent(ControlPort *port, float value, unsigned int bar, unsigned int position, unsigned int division) :
        Event(bar, position, division), port(port), value(value) {}
    ControlPort *getPort() {
        return port;
    }
    float getValue() {
        return value;
    }
};

class ControlConnection;

class ControlMapping : public Listable
{
    ControlConnection *const connection;
    const uint8_t cc;
    ControlPort *const port;
    const float minimum;
    const float maximum;
    float getScaled(unsigned char midiValue) {
        return minimum + (maximum - minimum) * midiValue / 127;
    }
public:
    ControlMapping(ControlConnection *connection, unsigned int cc,
                      ControlPort *port, float minimum, float maximum) :
        connection(connection), cc(cc), port(port), minimum(minimum), maximum(maximum) {
    }
    ControlConnection *getConnection() {
        return connection;
    }
    void update(uint8_t cc, uint8_t value) {
        if(this->cc == cc) {
            port->value = getScaled(value);
        }
    }
};

class ControlConnection : public Listable
{
    List<ControlMapping> mappings;
    midi::MidiConnection *connection;
public:
    ControlConnection(midi::MidiConnection *connection) :
        connection(connection) {}
    void addMapping(ControlMapping *mapping) { mappings.add(mapping); }
    void process(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time);
    void reset();
};

class State
{
    std::map<uint32_t, std::string> stateMap;
public:
    State(const char *filename);
    State(ScriptHashIterator &state);
    const void* retrieveState(uint32_t key, size_t* size, uint32_t* type, uint32_t* flags);
    const int getHash();
};

class Worker
{
    LV2_Handle handle;
    LV2_Worker_Interface *interface;
    jack_ringbuffer_t *requestBuffer;
    jack_ringbuffer_t *responseBuffer;
    char response[2048];
    pthread_t thread;
    sem_t semaphore;
public:
    Worker();
    void start();
    void run();
    LV2_Worker_Status scheduleWork(const void *, uint32_t size);
    LV2_Worker_Status queueResponse(const void *, uint32_t size);
    void respond();
    void setInstance(LilvInstance *instance);
};

class Plugin : public audio::Source, public midi::Source, public midi::Sink
{
    const LilvPlugin *plugin;
    LilvInstance *instance;
    // event inputs
    List<MidiInput> midiInputList;
    // event outputs
    uint32_t midiOutputCount;
    List<MidiOutput> midiOutputList;
    // audio inputs
    uint32_t audioInputCount;
    uint32_t *audioInputIndex;
    audio::AudioConnector *audioInput;
    // audio outputs
    uint32_t audioOutputCount;
    uint32_t *audioOutputIndex;
    audio::AudioConnection **audioOutput;
    // control ports
    EventBuffer<ControlEvent> controlBuffer;
    std::map<std::string, ControlPort *> controlMap;
    // worker
    Worker *worker;
    // control connections
    std::map<midi::MidiConnection*,ControlConnection*> controlConnectionMap;
    QueueList<ControlConnection> controlConnections;
    boost::lockfree::spsc_queue<ControlMapping*> newControlMappingsQueue;
public:
    static AtomTypes atomTypes;
    Plugin(const LilvPlugin *plugin, LilvInstance *instance, const Constants &uris, Worker *worker);
    ~Plugin();
    // public methods
    void connect(audio::Source &source);
    void connect(audio::AudioConnection *connection, uint32_t channel);
    void connectMidi(midi::Source &source);
    void setPortValue(const char*, const void*, uint32_t);
    void setControlValue(const char *symbol, float value);
    void scheduleControl(const char *symbol, float value, int bar, int position, int division);
    void addController(midi::Source &source, unsigned int cc, const char *symbol, float minimum, float maximum);
    void addController(midi::Source &source, unsigned int cc, const char *symbol, float minimum);
    void addController(midi::Source &source, unsigned int cc, const char *symbol);
    void restore();
    // MidiSink
    void addMidiEvent(midi::Event* evt);
    // Source interface
    bool connectsTo(AbstractSource *source);
    // Processor interface
    void doProcess(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time);
    void reposition();
    // AudioSource interface
    unsigned int getAudioOutputCount() { return audioOutputCount; }
    audio::AudioConnection *getAudioConnection(unsigned int index) {
        return audioOutput[index];
    }
    // MidiSource interface
    unsigned int getMidiOutputCount() { return midiOutputCount; }
    midi::MidiConnection *getMidiConnection(unsigned int index);
private:
    ControlPort *getPort(const char *symbol);
    void connectPort(int index, Plugin *other, int otherIndex);
    void print();
};

class PluginCache : public ProcessorCache<Plugin>
{
    LilvWorld* world;
    const LilvPlugins* plugins; // lilv_world_get_all_plugins
    jack_nframes_t sampleRate; // = AudioEngine::instance()->getSampleRate();
    const Constants lv2Constants;
    // for caching
    std::map<std::string, const LilvPlugin*> pluginMap;
    std::map<int, int> instanceCount;
    // for features
    const LV2_Feature* lv2Features[7];
    std::map<std::string, bool> supported;
    // options feature
    jack_nframes_t blockLength;
    LV2_Options_Option options[3];
    // urid features
    LV2_URID_Map map;
    LV2_URID_Unmap unmap;
    PathMapper pathMapper;
    LV2_State_Map_Path mapPath;
    // worker schedule feature
    LV2_Worker_Schedule schedule;
    // use instance()
    PluginCache();
    void scriptReset() {
        instanceCount.clear();
    }
public:
    UridMapper uridMapper;
    ~PluginCache();
    static PluginCache &instance() {
        static PluginCache instance;
        return instance;
    }
    void setSampleRate(jack_nframes_t rate) {
        this->sampleRate = rate;
    }
    void setBufferSize(jack_nframes_t size) {
        this->blockLength = size;
    }
    Plugin *getPlugin(const char *uri, const char *preset, State *state);
    Plugin *getPlugin(const char *uri, const char *preset) {
        return getPlugin(uri, preset, 0);
    }
    Plugin *getPlugin(const char *uri, State &state) {
        return getPlugin(uri, 0, &state);
    }
    Plugin *getPlugin(const char *uri) {
        return getPlugin(uri, 0, 0);
    }
};

}}

#endif // AUDIOPLUGIN_H
