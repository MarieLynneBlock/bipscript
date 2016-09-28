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

typedef std::map<std::string, uint32_t> Lv2UridMap;

class Lv2UridMapper
{
    Lv2UridMap uridMap;
public:
    LV2_URID uriToId(const char *uri);
    const char *idToUri(LV2_URID urid);
};

class Lv2PathMapper
{
public:
    char *mapAbsolutePath(const char *abstractPath);
    char *mapAbstractPath(const char *absolutePath);
};

class Lv2Constants
{
public:
    Lv2Constants(LilvWorld *);
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

class Lv2AtomTypes
{
public:
    LV2_URID doubleType;
    LV2_URID floatType;
    LV2_URID intType;
    LV2_URID longType;
    LV2_URID stringType;
};

class Lv2MidiEvent {
public:
    static uint32_t midiEventTypeId;
    LV2_Atom_Event event;
    uint8_t buffer[3];
    Lv2MidiEvent() {
        event.body.type = midiEventTypeId;
        event.body.size = 3;
    }
};

class Lv2MidiInput : public Listable
{
    const u_int32_t CAPACITY = 1024;
    LV2_Atom_Sequence *atomSequence;
    MidiConnector eventConnector;
    EventBuffer eventBuffer;
    bool localRolling;
public:
    Lv2MidiInput() : localRolling(false) {
        atomSequence = static_cast<LV2_Atom_Sequence *>(malloc(sizeof(LV2_Atom_Sequence) + CAPACITY));
    }
    LV2_Atom_Sequence *getAtomSequence() {
        return atomSequence;
    }
    void connect(MidiConnection *connection, Source *source) {
        eventConnector.setConnection(connection, source);
    }
    bool connectsTo(Source *source) {
        MidiConnection *connection = eventConnector.getConnection();
        return connection && connection->connectsTo(source);
    }
    void addEvent(MidiEvent *evt) {
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

class Lv2MidiOutput : public Listable, public MidiConnection 
{
    const u_int32_t CAPACITY = 1024;
    LV2_Atom_Sequence *atomSequence;    
    MidiEvent event;
public:
    Lv2MidiOutput(MidiSource *source) : MidiConnection(source) {
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
    MidiEvent *getEvent(uint32_t i);
};

class Lv2ControlPort {
public:
    Lv2ControlPort() : value(0) {}
    float value;
    float dfault;
    float minimum;
    float maximum;
};

class Lv2ControlEvent : public Event {
    Lv2ControlPort *port;
    float value;
public:
    Lv2ControlEvent(Lv2ControlPort *port, float value, unsigned int bar, unsigned int position, unsigned int division) :
        Event(bar, position, division), port(port), value(value) {}
    Lv2ControlPort *getPort() {
        return port;
    }
    float getValue() {
        return value;
    }
};

class Lv2ControlConnection;

class Lv2ControlMapping : public Listable
{
    Lv2ControlConnection *const connection;
    const uint8_t cc;
    Lv2ControlPort *const port;
    const float minimum;
    const float maximum;
    float getScaled(unsigned char midiValue) {
        return minimum + (maximum - minimum) * midiValue / 127;
    }
public:
    Lv2ControlMapping(Lv2ControlConnection *connection, unsigned int cc,
                      Lv2ControlPort *port, float minimum, float maximum) :
        connection(connection), cc(cc), port(port), minimum(minimum), maximum(maximum) {
    }
    Lv2ControlConnection *getConnection() {
        return connection;
    }
    void update(uint8_t cc, uint8_t value) {
        if(this->cc == cc) {
            port->value = getScaled(value);
        }
    }
};

class Lv2ControlConnection : public Listable
{
    List<Lv2ControlMapping> mappings;
    MidiConnection *connection;
public:
    Lv2ControlConnection(MidiConnection *connection) :
        connection(connection) {}
    void addMapping(Lv2ControlMapping *mapping) { mappings.add(mapping); }
    void process(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time);
    void reset();
};

class Lv2State
{
    std::map<uint32_t, std::string> stateMap;
public:
    Lv2State(const char *filename);
    Lv2State(ScriptHashIterator &state);
    const void* retrieveState(uint32_t key, size_t* size, uint32_t* type, uint32_t* flags);
    const int getHash();
};

class Lv2Worker
{
    LV2_Handle handle;
    LV2_Worker_Interface *interface;
    jack_ringbuffer_t *requestBuffer;
    jack_ringbuffer_t *responseBuffer;
    char response[2048];
    pthread_t thread;
    sem_t semaphore;
public:
    Lv2Worker();
    void start();
    void run();
    LV2_Worker_Status scheduleWork(const void *, uint32_t size);
    LV2_Worker_Status queueResponse(const void *, uint32_t size);
    void respond();
    void setInstance(LilvInstance *instance);
};

class Lv2Plugin : public Listable, public AudioSource, public MidiSource, public MidiSink
{
    const LilvPlugin *plugin;
    LilvInstance *instance;
    jack_nframes_t processedUntil;
    // event inputs
    List<Lv2MidiInput> midiInputList;
    // event outputs
    uint32_t midiOutputCount;
    List<Lv2MidiOutput> midiOutputList;
    // audio inputs
    uint32_t audioInputCount;
    uint32_t *audioInputIndex;
    AudioConnector *audioInput;
    // audio outputs
    uint32_t audioOutputCount;
    uint32_t *audioOutputIndex;
    AudioConnection **audioOutput;
    // control ports
    EventBuffer controlBuffer;
    std::map<std::string, Lv2ControlPort *> controlMap;
    // worker
    Lv2Worker *worker;
    // control connections
    std::map<MidiConnection*,Lv2ControlConnection*> controlConnectionMap;
    QueueList<Lv2ControlConnection> controlConnections;
    boost::lockfree::spsc_queue<Lv2ControlMapping*> newControlMappingsQueue;
public:
    static Lv2AtomTypes atomTypes;
    Lv2Plugin(const LilvPlugin *plugin, LilvInstance *instance, const Lv2Constants &uris, Lv2Worker *worker);
    ~Lv2Plugin();
    // public methods
    void connect(AudioSource &source);
    void connect(AudioConnection *connection, uint32_t channel);
    void connectMidi(MidiSource &source);
    void setPortValue(const char*, const void*, uint32_t);
    void setControlValue(const char *symbol, float value);
    void scheduleControl(const char *symbol, float value, int bar, int position, int division);
    void addController(MidiSource &source, unsigned int cc, const char *symbol, float minimum, float maximum);
    void addController(MidiSource &source, unsigned int cc, const char *symbol, float minimum);
    void addController(MidiSource &source, unsigned int cc, const char *symbol);
    void restore();
    void reposition();
    void processAll(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time);
    // MidiSink
    void addMidiEvent(MidiEvent* evt);
    // Source interface
    bool connectsTo(Source *source);
    void process(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time);
    // AudioSource interface
    unsigned int getAudioOutputCount() { return audioOutputCount; }
    AudioConnection *getAudioConnection(unsigned int index) {
        return audioOutput[index];
    }
    // MidiSource interface
    unsigned int getMidiOutputCount() { return midiOutputCount; }
    MidiConnection &getMidiConnection(unsigned int index);
private:
    Lv2ControlPort *getPort(const char *symbol);
    void connectPort(int index, Lv2Plugin *other, int otherIndex);
    void print();
};

class Lv2PluginCache : public ProcessorCache<Lv2Plugin>
{
    LilvWorld* world;
    const LilvPlugins* plugins; // lilv_world_get_all_plugins
    jack_nframes_t sampleRate; // = AudioEngine::instance()->getSampleRate();
    const Lv2Constants lv2Constants;
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
    Lv2PathMapper pathMapper;
    LV2_State_Map_Path mapPath;
    // worker schedule feature
    LV2_Worker_Schedule schedule;
    // use instance()
    Lv2PluginCache();
    void scriptReset() {
        instanceCount.clear();
    }
public:
    Lv2UridMapper uridMapper;
    ~Lv2PluginCache();
    static Lv2PluginCache &instance() {
        static Lv2PluginCache instance;
        return instance;
    }
    void setSampleRate(jack_nframes_t rate) {
        this->sampleRate = rate;
    }
    void setBufferSize(jack_nframes_t size) {
        this->blockLength = size;
    }
    Lv2Plugin *getPlugin(const char *uri, const char *preset, Lv2State *state);
    Lv2Plugin *getPlugin(const char *uri, const char *preset) {
        return getPlugin(uri, preset, 0);
    }
    Lv2Plugin *getPlugin(const char *uri, Lv2State &state) {
        return getPlugin(uri, 0, &state);
    }
    Lv2Plugin *getPlugin(const char *uri) {
        return getPlugin(uri, 0, 0);
    }
};

#endif // AUDIOPLUGIN_H
