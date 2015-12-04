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

#include <iostream>
#include <boost/filesystem.hpp>

#include "lv2plugin.h"
#include "objectcollector.h"
#include "scripthost.h"

#include "lv2/lv2plug.in/ns/ext/presets/presets.h"
#include "lv2/lv2plug.in/ns/ext/options/options.h"
#include "lv2/lv2plug.in/ns/ext/buf-size/buf-size.h"

namespace fs = boost::filesystem;

uint32_t Lv2MidiEvent::midiEventTypeId;

static LV2_URID uridMap(LV2_URID_Map_Handle handle, const char *uri) {
    return ((Lv2UridMapper*)handle)->uriToId(uri);
}

static const char* uridUnmap(LV2_URID_Unmap_Handle handle, LV2_URID urid) {
    return ((Lv2UridMapper*)handle)->idToUri(urid);
}

static char *mapAbsolutePath(LV2_State_Map_Path_Handle handle, const char *abstractPath) {
    return ((Lv2PathMapper*)handle)->mapAbsolutePath(abstractPath);
}

static char *mapAbstractPath(LV2_State_Map_Path_Handle handle, const char *absolutePath) {
    return ((Lv2PathMapper*)handle)->mapAbstractPath(absolutePath);
}

static void setPortValue(const char *port, void *data,
                         const void *value, uint32_t size, uint32_t type) {
    ((Lv2Plugin*)data)->setPortValue(port, value, type);
}

static const void* stateRetrieve(LV2_State_Handle handle,
                                 uint32_t key, size_t* size, uint32_t* type, uint32_t* flags) {
    return ((Lv2State*)handle)->retrieveState(key, size, type, flags);
}

LV2_URID Lv2UridMapper::uriToId(const char *uri) {
    const std::string uriString(uri);
    const Lv2UridMap::const_iterator iter = uridMap.find(uriString);
    if(iter == uridMap.end()) {
        const uint32_t newId = uridMap.size() + 10;
        uridMap.insert(std::make_pair(uriString, newId));
        //std::cout << "uridMap storing for " << uri << ": " << newId << std::endl;
        return newId;
    }
    //std::cout << "uridMap returning for " << uri << ": " << iter->second << std::endl;
    return iter->second;
}

const char *Lv2UridMapper::idToUri(LV2_URID urid) {
    Lv2UridMap::iterator iter;
    for(iter = uridMap.begin(); iter != uridMap.end(); iter++) {
        if(iter->second == urid) {
            //std::cout << "uridUnMap returning for " << urid << ": " << iter->first << std::endl;
            return iter->first.c_str();
        }
    }
    std::cerr << "!!! uridUnMap failed to find uri for " << urid << std::endl;
    return NULL;
}


char *Lv2PathMapper::mapAbsolutePath(const char *abstractPath)
{
    // get script folder and append incoming relative path
    fs::path absolutePath(ScriptHost::instance().getCurrentFolder());
    absolutePath /= abstractPath;
    absolutePath = canonical(absolutePath);
    // copy to malloc-allocated char*
    const char *pathString = absolutePath.c_str();
    char *ret = (char *)malloc(strlen(pathString) + 1);
    strcpy(ret, pathString);
    return ret;
}

char *Lv2PathMapper::mapAbstractPath(const char *absolutePath)
{
    // TODO: implement before implementing state save
    char *ret = (char *)malloc(strlen(absolutePath) + 1);
    strcpy(ret, absolutePath);
    return ret;
}

Lv2Constants::Lv2Constants(LilvWorld *world) {
    lv2AtomPort = lilv_new_uri(world, LV2_ATOM__AtomPort);
    lv2AudioPort = lilv_new_uri(world, LV2_CORE__AudioPort);
    lv2InputPort = lilv_new_uri(world, LV2_CORE__InputPort);
    lv2OutputPort = lilv_new_uri(world, LV2_CORE__OutputPort);
    lv2ControlPort = lilv_new_uri(world, LV2_CORE__ControlPort);
    lv2MidiEvent = lilv_new_uri(world, LILV_URI_MIDI_EVENT);
    lv2AtomSequence = lilv_new_uri(world, LV2_ATOM__Sequence);
    lv2AtomSupports = lilv_new_uri(world, LV2_ATOM__supports);
    lv2AtomBufferType = lilv_new_uri(world, LV2_ATOM__bufferType);
    lv2Presets = lilv_new_uri(world, LV2_PRESETS__Preset);
    lv2RdfsLabel = lilv_new_uri(world, LILV_NS_RDFS "label");
}

// ----------------------------- Lv2MidiInput

/**
 * process this MIDI input: pack LV2 input buffer from EventBuffer + EventConnection
 */
void Lv2MidiInput::process(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time) {
    // clear atom sequence
    lv2_atom_sequence_clear(atomSequence);

    // send all notes off messages if newly stopped
    if(localRolling && !rolling) {
        Lv2MidiEvent lv2Event;
        lv2Event.event.time.frames = 0;
        lv2Event.buffer[1] = 0x7b; // CC 123 = all notes off
        lv2Event.buffer[2] = 0;
        for(uint8_t channel = 0; channel < 16; channel++) {
            lv2Event.buffer[0] = MidiEvent::TYPE_CONTROL + channel;
            lv2_atom_sequence_append_event(atomSequence, CAPACITY, &lv2Event.event);
        }
    }
    localRolling = rolling;

    // get connection and event count
    EventConnection *connection = eventConnector.getConnection();
    uint32_t eventCount = 0;
    if(connection) {
        connection->process(rolling, pos, nframes, time);
        eventCount = connection->getEventCount();
    }

    // get top events from buffer + connection
    MidiEvent* bufferEvent = static_cast<MidiEvent*>(eventBuffer.getNextEvent(rolling, pos, nframes));
    MidiEvent *connectionEvent = eventCount ? connection->getEvent(0) : 0;
    uint32_t eventIndex = 1;

    // loop while events on either buffer or connection
    while(bufferEvent || connectionEvent) {
        bool bufferNext = bufferEvent &&
                (!connectionEvent || bufferEvent->getFrameOffset() < connectionEvent->getFrameOffset());
        MidiEvent *event = bufferNext ? bufferEvent : connectionEvent;
        // lv2 event
        Lv2MidiEvent lv2Event;
        long frame = event->getFrameOffset();
        lv2Event.event.time.frames = frame >= 0 ? frame : 0;
        // copy event data
        event->pack(&lv2Event.buffer);
        // append this event to sequence
        lv2_atom_sequence_append_event(atomSequence, CAPACITY, &lv2Event.event);
        // get next event
        if(bufferNext) {
            // recycle and get next buffer event
            ObjectCollector::instance()->recycle(bufferEvent);
            bufferEvent = static_cast<MidiEvent*>(eventBuffer.getNextEvent(rolling, pos, nframes));
        } else {
            connectionEvent = eventIndex < eventCount ? connection->getEvent(eventIndex++) : 0;
        }
    }
}

// ----------------------------- Lv2MidiOutput

uint32_t Lv2MidiOutput::getEventCount()
{
    uint32_t counter = 0;
    LV2_ATOM_SEQUENCE_FOREACH(atomSequence, ev) {
        if (ev->body.type == Lv2MidiEvent::midiEventTypeId) {
            counter++;
        }
    }
    return counter;
}

MidiEvent *Lv2MidiOutput::getEvent(uint32_t i)
{
    // TODO: direct access?
    uint32_t counter = 0;
    LV2_ATOM_SEQUENCE_FOREACH(atomSequence, ev) {
        if (ev->body.type == Lv2MidiEvent::midiEventTypeId) {
            if(counter == i) {
                event.unpack((const uint8_t*)(ev + 1));
                event.setFrameOffset(ev->time.frames);
                return &event;
            }
            counter++;
        }
    }
    return 0;
}

void Lv2ControlConnection::process(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time)
{
    connection->process(rolling, pos, nframes, time);
    u_int32_t eventIndex = 0;
    u_int32_t eventCount = connection->getEventCount();
    while(eventIndex < eventCount) {
        MidiEvent *nextEvent = connection->getEvent(eventIndex);
        if(nextEvent->matches(MidiEvent::TYPE_CONTROL)) {
            // check mappings
            Lv2ControlMapping *mapping = mappings.getFirst();
            while(mapping) {
                mapping->update(nextEvent->getDatabyte1(), nextEvent->getDatabyte2());
                mapping = mappings.getNext(mapping);
            }
        }
        // pull up next event
        nextEvent = eventIndex < eventCount ? connection->getEvent(eventIndex++) : 0;
    }
}

void Lv2ControlConnection::reset()
{
    // recycle mappings
    Lv2ControlMapping *mapping = mappings.getFirst();
    while (mapping) {
        Lv2ControlMapping *done = mapping;
        mapping = mappings.pop();
        ObjectCollector::instance()->recycle(done);
    }
}

// ----------------------------- Lv2Plugin

Lv2Plugin::Lv2Plugin(const LilvPlugin *plugin, LilvInstance *instance, const Lv2Constants &uris) :
    plugin(plugin), instance(instance), processedUntil(0),
    controlConnections(4), newControlMappingsQueue(16)
{
    // audio inputs
    audioInputCount = lilv_plugin_get_num_ports_of_class(plugin, uris.lv2AudioPort, uris.lv2InputPort, 0);
    audioInputIndex = new uint32_t[audioInputCount];
    audioInput = new AudioConnector[audioInputCount];

    // audio outputs
    audioOutputCount = lilv_plugin_get_num_ports_of_class(plugin, uris.lv2AudioPort, uris.lv2OutputPort, 0);
    audioOutputIndex = new uint32_t[audioOutputCount];
    audioOutput = new AudioConnection*[audioOutputCount];
    for(uint32_t i = 0; i < audioOutputCount; i++) {
        audioOutput[i] = new AudioConnection(this);
        audioOutput[i]->clear();
    }

    // initialize port structures
    uint32_t numPorts = lilv_plugin_get_num_ports(plugin);
    uint32_t audioInputCounter = 0;
    uint32_t audioOutputCounter = 0;
    for(uint32_t i = 0; i < numPorts; i++) {
        const LilvPort *port = lilv_plugin_get_port_by_index(plugin, i);
        if(lilv_port_is_a(plugin, port, uris.lv2AudioPort)) {
            if(lilv_port_is_a(plugin, port, uris.lv2InputPort)) {
                audioInputIndex[audioInputCounter++] = i;
            }
            else if(lilv_port_is_a(plugin, port, uris.lv2OutputPort)) {
                audioOutputIndex[audioOutputCounter++] = i;
            }
        } else if(lilv_port_is_a(plugin, port, uris.lv2ControlPort)) {

            // get control name
            const LilvNode* symbol = lilv_port_get_symbol(plugin, port);
            std::string portName(lilv_node_as_string(symbol));

            // create, connect and hash new control port object
            Lv2ControlPort *newPort = new Lv2ControlPort();
            LilvNode *dfault, *minimum, *maximum;
            lilv_port_get_range(plugin, port, &dfault, &minimum, &maximum);
            newPort->dfault = dfault ? lilv_node_as_float(dfault) : 0;
            newPort->minimum = lilv_node_as_float(minimum);
            newPort->maximum = lilv_node_as_float(maximum);
            lilv_instance_connect_port(instance, i, &(newPort->value));
            controlMap[portName] = newPort;

        } else if(lilv_port_is_a(plugin, port, uris.lv2AtomPort)) {
            // is it a MIDI/atom input?
            LilvNodes *atomBufferType = lilv_port_get_value(plugin, port, uris.lv2AtomBufferType);
            LilvNodes* atomSupports = lilv_port_get_value(plugin, port, uris.lv2AtomSupports);
            if (lilv_port_is_a(plugin, port, uris.lv2InputPort) &&
                    lilv_nodes_contains(atomBufferType, uris.lv2AtomSequence)
                    && lilv_nodes_contains(atomSupports, uris.lv2MidiEvent)) {
                // create new inputs and connect to atom sequence location
                Lv2MidiInput *newAtomPort = new Lv2MidiInput();
                lilv_instance_connect_port(instance, i, newAtomPort->getAtomSequence());
                midiInputList.add(newAtomPort);
            }
            else if (lilv_port_is_a(plugin, port, uris.lv2OutputPort)) {
                //atomSequence->atom.type = Lv2PluginFactory::instance()->uridMapper.uriToId(LV2_ATOM__Sequence);
                Lv2MidiOutput *midiOutput = new Lv2MidiOutput(this);
                lilv_instance_connect_port(instance, i, midiOutput->getAtomSequence());
                midiOutputList.add(midiOutput);
                midiOutputCount++;
            }
            else {
                // warn
                std::cout << "!!! unknown atom port at index " << i << ": " << lilv_node_as_string(lilv_port_get_name(plugin, port)) << std::endl;
            }
            lilv_nodes_free(atomSupports);
            lilv_nodes_free(atomBufferType);
        } else {
            lilv_instance_connect_port(instance, i, NULL);
            std::cout << "!!! unknown port at index " << i << ": " << lilv_node_as_string(lilv_port_get_name(plugin, port)) << std::endl;
        }
    }
}

Lv2Plugin::~Lv2Plugin()
{
    lilv_instance_free(instance);
}

void Lv2Plugin::connect(AudioSource &source)
{
    if(audioInputCount == 0) {
        throw std::logic_error("cannot connect: this plugin has no audio inputs");
    }
    if(audioInputCount == 2 && source.getAudioOutputCount() == 1) { // mono to stereo
        audioInput[0].setConnection(source.getAudioConnection(0), this);
        audioInput[1].setConnection(source.getAudioConnection(0), this);
    }
    else if(audioInputCount == source.getAudioOutputCount()) {
        for(uint32_t i = 0; i < audioInputCount; i++) {
            audioInput[i].setConnection(source.getAudioConnection(i), this);
        }
    }
    else {
        throw std::logic_error("cannot connect: different number of input/output connections");
    }
}

void Lv2Plugin::connect(AudioConnection *connection, uint32_t channel)
{
    if(channel == 0) {
        throw std::logic_error("cannot connect: there is no channel zero");
    }
    if(channel > audioInputCount) {
        throw std::logic_error("cannot connect: channel too high");
    }
    audioInput[channel - 1].setConnection(connection, this);
}

void Lv2Plugin::connectMidi(EventSource &source)
{
    if(source.getEventOutputCount() == 0) {
        throw std::logic_error("cannot connect: the source has no MIDI outputs");
    }
    Lv2MidiInput *input = midiInputList.getFirst();
    if(input) {
        input->connect(&source.getEventConnection(0), this);
    }
    else {
        throw std::logic_error("cannot connect: this plugin has no MIDI inputs");
    }
}

Lv2State::Lv2State(const char *filename)
{
    throw std::logic_error("LV2 State from file not yet implemented"); // TODO implement
}

Lv2State::Lv2State(ScriptHashIterator &stateHash)
{
    Lv2UridMapper &uridMapper = Lv2PluginCache::instance().uridMapper;
    while(stateHash.hasNext()) {
        ScriptHashPair &pair = stateHash.next();
        if(pair.key.type != STRING || pair.value.type != STRING) {
            throw std::logic_error("setState: key and value must be of type string");
        }
        uint32_t key = uridMapper.uriToId(pair.key.stringValue);
        stateMap[key] = pair.value.stringValue;
    }
}

const void *Lv2State::retrieveState(uint32_t key, size_t *size, uint32_t *type, uint32_t *flags)
{
    *type = Lv2Plugin::atomTypes.stringType;
    if(stateMap.find( key ) != stateMap.end()) {
        std::string &value = stateMap[key];
        *size = value.length();
        return value.c_str();
    } else {
        *size = 0;
        return 0;
    }
}

void Lv2Plugin::setPortValue(const char *portname, const void *value, uint32_t type) {
    Lv2ControlPort *lv2Port = controlMap[portname];
    if(lv2Port) {
        if (type == atomTypes.floatType) {
            lv2Port->value = *(const float *)value;
        } else if (type == atomTypes.doubleType) {
            lv2Port->value = *(const double *)value;
        } else if (type == atomTypes.intType) {
            lv2Port->value = *(const int32_t *)value;
        } else if (type == atomTypes.longType) {
            lv2Port->value = *(const int64_t *)value;
        } else {
            std::cerr << "warning: attempt to set control port with unknown type: " << type << std::endl;
        }
    } else {
        std::cerr << "warning: attempt to set unknown control port '" << portname << "'" << std::endl;
    }
}

/**
 * Looks up an Lv2ControlPort pointer that corresponds to a string symbol
 *
 * Runs in the script thread.
 */
Lv2ControlPort *Lv2Plugin::getPort(const char *symbol) {
    Lv2ControlPort *port = controlMap[symbol];
    if(!port) {
        throw std::logic_error(std::string("Plugin has no such port: ") + symbol);
    }
    return port;
}

/**
 * Update the Lv2ControlPort value.
 *
 * Runs in the script thread.
 */
void Lv2Plugin::setControlValue(const char *symbol, float value) {
    getPort(symbol)->value = value;
}

/**
 * Schedule a control value change.
 *
 * Runs in the script thread.
 *
 * Allocates Lv2ControlEvent objects.
 */
void Lv2Plugin::scheduleControl(const char *symbol, float value, int bar, int position, int division) {
    Lv2ControlPort *port = getPort(symbol);
    controlBuffer.addEvent(new Lv2ControlEvent(port, value, bar, position, division));
}

/**
 * Add a control
 */

void Lv2Plugin::addController(EventSource &source, unsigned int cc, const char *symbol, float minimum, float maximum) {
    if(cc == 0) {
        throw std::logic_error("There is no MIDI control number zero");
    }
    if(cc > 127) {
        throw std::logic_error("MIDI control number cannot be greater than 127");
    }
    if(minimum > maximum) {
        throw std::logic_error("Minimum cannot be greater than maximum");
    }
    Lv2ControlPort *port = getPort(symbol);
    EventConnection &connection = source.getEventConnection(0); // TODO: how to specify other connections
    // check hash for existing connection
    Lv2ControlConnection *controlConnection = controlConnectionMap[&connection];
    if(!controlConnection) {
        // push new connection
        controlConnection = new Lv2ControlConnection(&connection);
        controlConnectionMap[&connection] = controlConnection;
        controlConnections.add(controlConnection);
    }
    // push new mapping
    while(!newControlMappingsQueue.push(new Lv2ControlMapping(controlConnection, cc, port, minimum, maximum)));
}

void Lv2Plugin::addController(EventSource &source, unsigned int cc, const char *symbol, float minimum) {    
    Lv2ControlPort *port = getPort(symbol);
    addController(source, cc, symbol, minimum, port->maximum);
}

void Lv2Plugin::addController(EventSource &source, unsigned int cc, const char *symbol) {
    Lv2ControlPort *port = getPort(symbol);
    addController(source, cc, symbol, port->minimum, port->maximum);
}

/**
 * Restore clean state on a cached plugin before reuse
 *
 * Runs in script thread.
 */
void Lv2Plugin::restore()
{
    controlConnectionMap.clear();
	// TODO: reset control values
}

void Lv2Plugin::addMidiEvent(MidiEvent *evt) {
    // for now just add to first midi port
    Lv2MidiInput *midiInput = midiInputList.getFirst();
    if(midiInput) {
        midiInput->addEvent(evt);
    }
}

bool Lv2Plugin::connectsTo(Source *source) {
    // event inputs
    Lv2MidiInput *midiInput = midiInputList.getFirst();
    while(midiInput) {
        if(midiInput->connectsTo(source)) {
            return true;
        }
        midiInput = midiInputList.getNext(midiInput);
    }
    // audio inputs
    for(uint32_t i = 0; i < audioInputCount; i++) {
        AudioConnection *connection = audioInput[i].getConnection();
        if(connection && connection->connectsTo(source)) {
            return true;
        }
    }
    return false;
}

EventConnection &Lv2Plugin::getEventConnection(unsigned int index) {
    unsigned int count = 0;
    Lv2MidiOutput *output = midiOutputList.getFirst();
    while(output && count < index) {
        output = midiOutputList.getNext(output);
        count++;
    }
    return *output;
}

void Lv2Plugin::process(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time) {

    // pull in new control mappings
    Lv2ControlMapping *freshMapping;
    while(newControlMappingsQueue.pop(freshMapping)) {
        freshMapping->getConnection()->addMapping(freshMapping);
    }

    // have we already processed this cycle
    if(processedUntil >= time) {
        return;
    }

    // process MIDI inputs
    Lv2MidiInput *midiInput = midiInputList.getFirst();
    while(midiInput) {
        midiInput->process(rolling, pos, nframes, time);
        midiInput = midiInputList.getNext(midiInput);
    }

    // process and connect audio inputs
    for(uint32_t i = 0; i < audioInputCount; i++) {
        AudioConnection *connection = audioInput[i].getConnection();
        if(connection) {
            float *audio = connection->getAudio(rolling, pos, nframes, time);
            lilv_instance_connect_port(instance, audioInputIndex[i], audio);
        } else {
            lilv_instance_connect_port(instance, audioInputIndex[i], AudioConnection::getDummyBuffer());
        }
    }

    // update control port values from scheduled control events
    Event* nextEvent = controlBuffer.getNextEvent(rolling, pos, nframes);
    while(nextEvent) {
        Lv2ControlEvent *evt = static_cast<Lv2ControlEvent*>(nextEvent);
        evt->getPort()->value = evt->getValue();
        ObjectCollector::instance()->recycle(nextEvent);
        nextEvent = controlBuffer.getNextEvent(rolling, pos, nframes);
    }

    // process control connections
    Lv2ControlConnection *connection = this->controlConnections.getFirst();
    while(connection) {
        connection->process(rolling, pos, nframes, time);
        connection = controlConnections.getNext(connection);
    }

    // clear event output buffers
    Lv2MidiOutput *midiOutput = midiOutputList.getFirst();
    while(midiOutput) {
        midiOutput->clear();
        midiOutput = midiOutputList.getNext(midiOutput);
    }

    // set up audio output buffers
    for(uint32_t i = 0; i < audioOutputCount; i++) {
        lilv_instance_connect_port(instance, audioOutputIndex[i], audioOutput[i]->getBuffer());
    }

    // run the plugin
    lilv_instance_run(instance, nframes);

    // update the processed time
    processedUntil = time;
}

void Lv2Plugin::reposition() {
    // reset MIDI inputs
    Lv2MidiInput *midiInput = midiInputList.getFirst();
    while(midiInput) {
        midiInput->reset();
        midiInput = midiInputList.getNext(midiInput);
    }

    // recycle control connection
    Lv2ControlConnection *connection = controlConnections.getFirst();
    while(connection) {
        connection->reset();
        Lv2ControlConnection *done = connection;
        connection = controlConnections.pop();
        ObjectCollector::instance()->recycle(done);
    }

    // recycle control buffer
    controlBuffer.recycleRemaining();
}

void Lv2Plugin::processAll(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time)
{
    // update midi inputs
    Lv2MidiInput *midiInput = midiInputList.getFirst();
    while(midiInput) {
        midiInput->update();
        midiInput = midiInputList.getNext(midiInput);
    }
    // pull in new control mappings
    Lv2ControlMapping *freshMapping;
    while(newControlMappingsQueue.pop(freshMapping)) {
        freshMapping->getConnection()->addMapping(freshMapping);
    }
    // update control buffer
    controlBuffer.update();
}

void Lv2Plugin::print() {

    std::cout << "----------- plugin " << this << std::endl;
    //std::cout << atomMidiInputs.size() << " event inputs and zero outputs" << std::endl;
//    for(uint i = 0; i < atomMidiInputs.size(); i++) {
//        std::cout << "event input port " << i << " has buffer " << &atomMidiInputs[i].buffer << std::endl;
//    }

//    std::cout << numAudioInputs << " audio inputs and " << numAudioOutputs << " outputs" << std::endl;
//    for(uint i = 0; i < numAudioInputs; i++) {
//        std::cout << "audio input port " << i << " is connected to " << audioInputs[i].output << std::endl;
//        // std::cout << "\thas input " << audioInputs[i].input << std::endl;
//    }

//    for(uint i = 0; i < numAudioOutputs; i++) {
//        std::cout << "audio output port " << i << " is " << &audioOutputs[i] << std::endl;
//    }
}


Lv2AtomTypes Lv2Plugin::atomTypes;

Lv2PluginCache::Lv2PluginCache() : world(lilv_world_new()), lv2Constants(world) {

    // plugins
    lilv_world_load_all(world);
    plugins = lilv_world_get_all_plugins(world);

    // supported features
    supported[LV2_URID__map] = true;
    supported[LV2_URID__unmap] = true;
    supported[LV2_OPTIONS__options] = true;
    supported[LV2_BUF_SIZE__boundedBlockLength] = true;
    supported[LV2_STATE__mapPath] = true;

    // URID feature (map/unmap)
    map.map = &uridMap;
    map.handle = &uridMapper;
    static LV2_Feature uridMapFeature  = { LV2_URID__map, &map };
    lv2Features[0] = &uridMapFeature;

    unmap.unmap = &uridUnmap;
    unmap.handle = &uridMapper;
    static LV2_Feature uridUnmapFeature  = { LV2_URID__unmap, &unmap };
    lv2Features[1] = &uridUnmapFeature;

    // Options feature
    jack_nframes_t blockLength = AudioConnection::getBufferSize();
    LV2_Options_Option options[] = {
        {   LV2_OPTIONS_INSTANCE, 0, uridMapper.uriToId(LV2_BUF_SIZE__minBlockLength),
            sizeof(float), uridMapper.uriToId(LV2_ATOM__Int), &blockLength
        },
        {   LV2_OPTIONS_INSTANCE, 0, uridMapper.uriToId(LV2_BUF_SIZE__maxBlockLength),
            sizeof(float), uridMapper.uriToId(LV2_ATOM__Int), &blockLength
        },
        { LV2_OPTIONS_INSTANCE, 0, 0, 0, 0, NULL }
    };

    static LV2_Feature optionsFeature = { LV2_OPTIONS__options, &options };
    lv2Features[2] = &optionsFeature;

    static LV2_Feature boundedBlockFeature = { LV2_BUF_SIZE__boundedBlockLength, NULL };
    lv2Features[3] = &boundedBlockFeature;

    // state map path feature
    mapPath.handle = &pathMapper;
    mapPath.absolute_path = &mapAbsolutePath;
    mapPath.abstract_path = &mapAbstractPath;
    static LV2_Feature stateMapPathFeature = { LV2_STATE__mapPath, &mapPath };
    lv2Features[4] = &stateMapPathFeature;

    // end of features
    lv2Features[5] = 0;

    // URIDs
    Lv2Plugin::atomTypes.intType = uridMapper.uriToId(LV2_ATOM__Int);
    Lv2Plugin::atomTypes.longType = uridMapper.uriToId(LV2_ATOM__Long);
    Lv2Plugin::atomTypes.floatType = uridMapper.uriToId(LV2_ATOM__Float);
    Lv2Plugin::atomTypes.doubleType = uridMapper.uriToId(LV2_ATOM__Double);
    Lv2Plugin::atomTypes.stringType = uridMapper.uriToId(LV2_ATOM__String);
    Lv2MidiEvent::midiEventTypeId = uridMapper.uriToId(LILV_URI_MIDI_EVENT);
}


Lv2Plugin *Lv2PluginCache::getPlugin(const char *uri, const char *preset, Lv2State *state) {
    // create a unique key for uri/preset/state
    std::string keyString(uri);
    keyString.append(":");
    if(preset) {
        keyString.append(preset);
    }
    keyString.append(":");
    if(state) {
        keyString.append(state->getHash());
    }

    // count tells how many instances of this type of plugin
    int count = ++instanceCount[keyString];

    // return cached plugin instance if available
    std::string key = keyString + ":" + std::to_string(count);
    Lv2Plugin *cachedPlugin = findObject(key);
    if(cachedPlugin) {
        cachedPlugin->restore();
        return cachedPlugin;
    }

    // look in cache for plugin type by uri
    std::string uriString(uri);
    const LilvPlugin *lilvPlugin = pluginMap[uriString];
    if(!lilvPlugin) {
        // find lv2 plugin in lilv
        LilvNode *lilvUri = lilv_new_uri(world, uri);
        lilvPlugin = lilv_plugins_get_by_uri(plugins, lilvUri);
        lilv_node_free(lilvUri);

        if(!lilvPlugin) {
            throw std::logic_error(std::string("Plugin is not installed on this system: ") + uriString);
        }

        // check that required features are supported
        LilvNodes* features = lilv_plugin_get_required_features(lilvPlugin);
        for (LilvIter* f = lilv_nodes_begin(features); !lilv_nodes_is_end(features, f); f = lilv_nodes_next(features, f)) {
            const char* featureUri = lilv_node_as_uri(lilv_nodes_get(features, f));
            if(!supported[featureUri]) {
                throw std::logic_error(std::string("Plugin ") + uriString + " requires unsupported feature: " + featureUri);
            }
        }
        lilv_nodes_free(features);
        pluginMap[uriString] = lilvPlugin;
    }

    // instantiate
    LilvInstance *instance = lilv_plugin_instantiate(lilvPlugin, sampleRate, lv2Features);

    // create plugin object
    Lv2Plugin *plugin = new Lv2Plugin(lilvPlugin, instance, lv2Constants);

    // restore baseline default state
    LilvState *defaultState =  lilv_state_new_from_world(world, &map, lilv_plugin_get_uri(lilvPlugin));
    lilv_state_restore(defaultState, instance, setPortValue, plugin, 0, lv2Features);

    // find and restore preset
    if(preset) {
        LilvNodes* presets = lilv_plugin_get_related(lilvPlugin, lv2Constants.lv2Presets);
        LilvNode *myPreset = 0;
        LILV_FOREACH(nodes, i, presets) {
            const LilvNode* presetNode = lilv_nodes_get(presets, i);
            lilv_world_load_resource(world, presetNode);
            LilvNodes* labels = lilv_world_find_nodes(world, presetNode, lv2Constants.lv2RdfsLabel, NULL);
            if (labels) {
                const LilvNode* label = lilv_nodes_get_first(labels);
                const char *labelString = lilv_node_as_string(label); // TODO: free?
                if(!strcmp(labelString, preset)) {
                    myPreset = lilv_node_duplicate(presetNode);
                }
                lilv_nodes_free(labels);
            }
        }
        lilv_nodes_free(presets);
        if(myPreset) {
            LilvState* presetState = lilv_state_new_from_world(world, &map, myPreset);
            lilv_state_restore(presetState, instance, setPortValue, plugin, 0, NULL);
            // lilv_state_free(state); // TODO
        }
        else {
            throw std::logic_error(std::string("Plugin ") + uriString + " has no such preset: " + preset);
        }
    }
    // restore state
    else if(state) {
        LV2_State_Interface* iState = (LV2_State_Interface*)lilv_instance_get_extension_data(instance, LV2_STATE__interface);
        if (iState) {
            LV2_State_Status status = iState->restore(instance->lv2_handle, &stateRetrieve,
                                                      (LV2_State_Handle)state, 0, lv2Features);
            if(status != LV2_STATE_SUCCESS) {
                throw std::logic_error(std::string("Plugin ") + uriString
                                       + " setting state failed with code " + std::to_string(status));
            }
        }
    }

    // add to cache
    registerObject(key, plugin);

    // activate plugin
    lilv_instance_activate(instance);    
    return plugin;
}

Lv2PluginCache::~Lv2PluginCache()
{
    lilv_world_free(world);
}
