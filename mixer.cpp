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

#include "mixer.h"
#include "objectcollector.h"

#include <cstring>
#include <stdexcept>

/**
 * Process a control connection: calls process on the underlying EventConnection and resets that
 * connection's eventCount and eventIndex for this period.
 *
 * Runs in the process thread.
 */
void MixerControlConnection::process(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time)
{
    connection->process(rolling, pos, nframes, time);
    eventCount = connection->getEventCount();
    eventIndex = 0;
}

/**
 * Check the mappings vs the incoming events on this connection to see if there are any applicable
 * gain change events for this period, if so then make the change directly on the gain matrix.
 *
 * Runs in the process thread.
 *
 */
void MixerControlConnection::updateGains(jack_nframes_t frame, float **gain)
{
    if(eventCount && eventIndex < eventCount) {
        MidiEvent *nextEvent = connection->getEvent(eventIndex);
        while(nextEvent && nextEvent->getFrameOffset() == frame) {
            if(nextEvent->matches(MidiEvent::TYPE_CONTROL)) {
                // check mappings
                MixerControlMapping *mapping = mappings.getFirst();
                while(mapping) {
                    if(mapping->cc == nextEvent->getDatabyte1()) {
                        gain[mapping->input][mapping->output] = (float) nextEvent->getDatabyte2() / 127;
                    }
                    mapping = mappings.getNext(mapping);
                }
            }
            // pull up next event
            nextEvent = eventIndex < eventCount ? connection->getEvent(eventIndex++) : 0;
        }
    }
}

/**
 * Reset this MixerControlConnection.
 *
 * Runs in process thread.
 *
 * Recycles MixerControlMapping objects.
 */

void MixerControlConnection::reposition()
{
    // recycle mappings
    MixerControlMapping *mapping = mappings.getFirst();
    while (mapping) {
        MixerControlMapping *done = mapping;
        mapping = mappings.pop();
        ObjectCollector::instance()->recycle(done);
    }
}

/**
 *
 * Mixer constructor.
 *
 * Runs in script thread.
 *
 * Allocates AudioConnector, AudioConnection objects, routing and gain matrices.
 */

Mixer::Mixer(unsigned int inputs, const unsigned int outputs)
    : processedUntil(0), connectedInputs(0), audioInputCount(inputs),
      audioOutputCount(outputs), newControlMappingsQueue(16), controlConnections(4) {
    audioInput = new AudioConnector[inputs];
    audioOutput = new AudioConnection*[outputs];
    for(uint32_t i = 0; i < audioOutputCount; i++) {
        audioOutput[i] = new AudioConnection(this);
    }
    gain = new float*[inputs];
    for(unsigned int i = 0; i < inputs; i++) {
        gain[i] = new float[outputs]();        
    }
}

/**
 * Mixer dtor.
 *
 * Runs in script thread.
 *
 * Deletes AudioConnector, AudioConnection objects, routing and gain matrices.
 *
 */
Mixer::~Mixer()
{
    delete[] audioInput;
    for(uint32_t i = 0; i < audioOutputCount; i++) {
        delete audioOutput[i];
    }
    delete[] audioOutput;
    for(unsigned int i = 0; i < audioInputCount; i++) {
        delete[] gain[i];
    }
    delete[] gain;
}

/**
 * Connect an AudioSource.
 *
 * Runs in script thread.
 *
 * No allocations.
 */
void Mixer::nextAudioInput(AudioSource &source)
{
    unsigned int inputOutputCount = source.getAudioOutputCount();
    // source has no audio outputs
    if(!inputOutputCount) {
        throw std::logic_error("input has no audio outputs");
    }
    // not enough free inputs
    if(connectedInputs + inputOutputCount > audioInputCount) {
        throw std::logic_error("not enough inputs available");
    }
    // loop over inputs and assign to outputs
    for(unsigned int i = 0; i < inputOutputCount; i++) {
        gain[connectedInputs][i % this->getAudioOutputCount()] = 1.0;
        audioInput[connectedInputs++].setConnection(&source.getAudioConnection(i), this);
    }
}

/**
 * Connect an AudioSource.
 *
 * Runs in script thread.
 *
 * No allocations.
 */
void Mixer::nextStereoInput(AudioSource &source)
{
    unsigned int inputOutputCount = source.getAudioOutputCount();
    // source has no audio outputs
    if(!inputOutputCount) {
        throw std::logic_error("input has no audio outputs");
    }
    // not enough free inputs
    if(connectedInputs + 2 > audioInputCount) {
        throw std::logic_error("not enough inputs available");
    }
    // connect first channel
    gain[connectedInputs][0] = 1.0;
    audioInput[connectedInputs++].setConnection(&source.getAudioConnection(0), this);
    // connect second channel
    gain[connectedInputs][1] = 1.0;
    if(inputOutputCount == 1) {
        audioInput[connectedInputs++].setConnection(&source.getAudioConnection(0), this);
    } else {
        audioInput[connectedInputs++].setConnection(&source.getAudioConnection(1), this);
    }
 }

/**
 * Adds a gain control mapping.
 *
 * Runs in the script thread.
 *
 * Allocates MixerControlMapping, MixerControlConnection objects.
 */

void Mixer::addGainController(EventSource &source, unsigned int cc, unsigned int input, unsigned int output)
{
    if(cc == 0) {
        throw std::logic_error("There is no MIDI control number zero");
    }
    if(cc > 127) {
        throw std::logic_error("MIDI control number cannot be greater than 127");
    }
    if(input == 0) {
        throw std::logic_error("There is no input channel zero");
    }
    if(input > audioInputCount) {
        throw std::logic_error("No such input on this mixer");
    }
    if(output == 0) {
        throw std::logic_error("There is no output channel zero");
    }
    if(output > audioOutputCount) {
        throw std::logic_error("No such output on this mixer");
    }
    EventConnection &connection = source.getEventConnection(0); // TODO: how to specify other connections
    // check hash for existing connection
    MixerControlConnection *mixerConnection = controlConnectionMap[&connection];
    if(!mixerConnection) {
        // push new connection
        mixerConnection = new MixerControlConnection(&connection);
        controlConnectionMap[&connection] = mixerConnection;
        controlConnections.add(mixerConnection);
    }
    // push new mapping
    MixerControlMapping *mapping = new MixerControlMapping(mixerConnection, cc, input - 1, output - 1);
    while(!newControlMappingsQueue.push(mapping));
}

/**
 * Schedule a gain change.
 *
 * Runs in script thread.
 *
 * Allocates MixerGainEvent.
 */

void Mixer::scheduleGain(uint32_t input, uint32_t output, float gain, uint32_t bar, uint32_t position, uint32_t division) {
    gainEventBuffer.addEvent(new MixerGainEvent(input - 1, output - 1, gain, bar, position, division));
}

/**
 * Restore clean state on a cached mixer before reuse
 *
 * Runs in script thread.
 *
 * No allocations.
 */
void Mixer::restore()
{
    controlConnectionMap.clear();
}

/**
 * Postprocess method, called even when disconnected.
 *
 * Runs in the process thread.
 */
void Mixer::processAll(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time)
{
    // pull in new control mappings
    MixerControlMapping *freshMapping;
    while(newControlMappingsQueue.pop(freshMapping)) {
        freshMapping->connection->addMapping(freshMapping);
    }
	// update gain buffer
    gainEventBuffer.update();
}

/**
 * Checks connection path to a source.
 *
 * Runs in script thread.
 */
bool Mixer::connectsTo(Source *source) {
    // TODO: event inputs
    // audio inputs
    for(uint32_t i = 0; i < connectedInputs; i++) {
        if(audioInput[i].getConnection()->connectsTo(source)) {
            return true;
        }
    }
    return false;
}

/**
 * Main process method for a mixer.
 *
 * Runs in process thread.
 *
 * Recycles MidiGainEvent.
 */

void Mixer::process(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time)
{    
    // pull in new control mappings
    MixerControlMapping *freshMapping;
    while(newControlMappingsQueue.pop(freshMapping)) {
        freshMapping->connection->addMapping(freshMapping);
    }

    // have we already processed this cycle
    if(processedUntil >= time) {
        return;
    }

    // get audio from input connections
    float *audio[connectedInputs];
    for(unsigned int i = 0; i < connectedInputs; i++) {
        audio[i] = audioInput[i].getConnection()->getAudio(rolling, pos, nframes, time);
    }

    // zero out all output channels
    for(unsigned int i = 0; i < audioOutputCount; i++) {
        memset(audioOutput[i]->getBuffer(), 0, nframes * sizeof(float));
    }

    // process control connections
    MixerControlConnection *connection = this->controlConnections.getFirst();
    while(connection) {
        connection->process(rolling, pos, nframes, time);
        connection = controlConnections.getNext(connection);
    }

    // grab first gain event
    MixerGainEvent *event = static_cast<MixerGainEvent*>(gainEventBuffer.getNextEvent(rolling, pos, nframes));

    // loop over frames
    for(jack_nframes_t frame = 0; frame < nframes; frame++) {

        // check for gain changes via controllers
        MixerControlConnection *connection = this->controlConnections.getFirst();
        while(connection) {
            connection->updateGains(frame, gain);
            connection = controlConnections.getNext(connection);
        }

        // check for gain changes via scheduled events
        while(event && event->getFrameOffset() == frame) {
            // update gain
            gain[event->getInput()][event->getOutput()] = event->getValue();
            // recycle and get next buffer event
            ObjectCollector::instance()->recycle(event);
            event = static_cast<MixerGainEvent*>(gainEventBuffer.getNextEvent(rolling, pos, nframes));
        }

        // loop over inputs
        for(unsigned int i = 0; i < connectedInputs; i++) {
            // loop over outputs
            for(unsigned int o = 0; o < audioOutputCount; o++) {
                if(gain[i][o]) {
                    float *buffer = audioOutput[o]->getBuffer();
                    buffer[frame] += audio[i][frame] * gain[i][o];
                }
            }
        }
    }
    // update the processed time
    processedUntil = time;
}

/**
 * Reset method.
 *
 * Runs in the process thread.
 *
 * Recycles MixerControlConnection objects.
 */
void Mixer::reposition()
{
    // recycle control connection
    MixerControlConnection *connection = controlConnections.getFirst();
    while(connection) {
        connection->reposition();
        MixerControlConnection *done = connection;
        connection = controlConnections.pop();
        ObjectCollector::instance()->recycle(done);
    }
    // gain event buffer
    gainEventBuffer.recycleRemaining();
    // control connections
    MixerControlMapping *mapping;
    while(newControlMappingsQueue.pop(mapping)) {}
    connectedInputs = 0;
}

/**
 * Factory method.
 *
 * Runs in the script thread.
 *
 * Allocates Mixer objects.
 */
Mixer *MixerCache::getMixer(uint16_t inputs, uint16_t outputs)
{
    std::string ioString(std::to_string(inputs) + ":" + std::to_string(outputs));
    // count tells how many instances of this mixer io shape
    int count = instanceCount[ioString];
    instanceCount[ioString] = count + 1;
    // key for this mixer instance
    std::string key = ioString + ":" + std::to_string(count);
    Mixer *mixer = findObject(key);
    if (mixer) {
        mixer->restore();
        return mixer;
    }
    mixer = new Mixer(inputs, outputs);
    registerObject(key, mixer);
    return mixer;
}
