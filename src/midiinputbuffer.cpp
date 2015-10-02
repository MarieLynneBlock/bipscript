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

#include "audioengine.h"
#include "midiinputbuffer.h"
#include "objectcollector.h"

MidiInputBuffer::MidiInputBuffer(EventSource &source) :
    inputBuffer(1000)//, clientWaiting(false),
    //activeMutex(PTHREAD_MUTEX_INITIALIZER),
    //inactiveCondition(PTHREAD_COND_INITIALIZER)
{
    eventInput.store(&source.getEventConnection(0));
    outputBuffer = (MidiEvent*)malloc(1000 * sizeof(MidiEvent));
    for(unsigned char i = 0; i < 128; i++) {
        controlValue[i] = 0;
    }
}

MidiInputBuffer::~MidiInputBuffer()
{
    free(outputBuffer);
}

//MidiEvent *MidiInputBuffer::getEvents(int bar, int *numEvents) {
//    Position start = Position(bar, 1, 1);
//    Position end = Position(bar + 1, 1, 1);
//    return getEvents(bar, start, end, numEvents);
//}

//MidiEvent *MidiInputBuffer::getEvents(int bar, int position, int division, int *numEvents) {
//    Position start = Position(bar, position, division);
//    Position end = Position(bar, position + 1, division);
//    return getEvents(bar, start, end, numEvents);
//}

//MidiEvent *MidiInputBuffer::getEvents(int bar, Position &start, Position &end, int *numEvents)
//{
//    // wait for critical section
//    int error = pthread_mutex_lock(&activeMutex);
//    if(error) {
//        std::cerr << "failed to acquire activeMutex" << std::endl;
//        return 0;
//    }
//    // request
//    this->controlRequest = 0;
//    this->start = start;
//    this->end = end;
//    // wait for result
//    this->clientWaiting = true;
//    while(clientWaiting) {
//         pthread_cond_wait(&inactiveCondition, &activeMutex);
//    }
//    (*numEvents) = eventCount;
//    pthread_mutex_unlock(&activeMutex);
//    // return output buffer
//    return outputBuffer;
//}


//Pattern *InputBuffer::getEvents(int measure, int position, int division)
//{
//    int numEvents;
//    MidiEvent *events = buffer->getEvents(measure, position, division, &numEvents);
//    Pattern *pattern = new Pattern(); // TODO: memory leak
//    MidiEvent *noteOnEvent[16][128];

//    for(int i = 0; i < 16; i++) {
//        for(int j = 0; j < 128; j++) {
//            noteOnEvent[i][j] = 0; // memset?
//        }
//    }

//    for(int i = 0; i < numEvents; i++) {
//        MidiEvent *evt = &events[i];
//        if(evt->matches(0x90)) { // note ON
//            noteOnEvent[evt->channel][evt->getDatabyte1()] = evt;
//        }
//        else if (evt->matches((0x80))) { // note OFF
//            MidiEvent *noteOn = noteOnEvent[evt->channel][evt->getDatabyte1()];
//            if(!noteOn) {
//                continue; // ignore if there wasn't a note ON
//            }
//            Duration duration = (Duration)evt->getStart() - noteOn->getStart();
//            // std::cout << evt->getStart() << " - " << noteOn->getStart() << " = " << duration << std::endl;
//            Note note = Note(noteOn->getDatabyte1(), noteOn->getDatabyte2(), duration);
//            Position adjusted = noteOn->getStart() - measure;
//            // std::cout << evt->getStart() << " - " << measure << " = " << adjusted << std::endl;
//            pattern->addNote(note, adjusted);
//            noteOnEvent[evt->channel][evt->getDatabyte1()] = 0;
//        }
//    }
//    return pattern;
//}

unsigned int MidiInputBuffer::getLastControl(int control)
{
//    // wait for critical section
//    int error = pthread_mutex_lock(&activeMutex);
//    if(error) {
//        std::cerr << "failed to acquire activeMutex" << std::endl;
//        return 0;
//    }
//    // request
//    this->controlRequest = control;
//    this->end = Position(bar, position + 1, division);
//    // wait for result
//    this->clientWaiting = true;
//    while(clientWaiting) {
//         pthread_cond_wait(&inactiveCondition, &activeMutex);
//    }
//    pthread_mutex_unlock(&activeMutex);
    // return output buffer
    return controlValue[control].load();
}

void MidiInputBuffer::processAll(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time)
{
//    // DEBUG
//    if(eventCount) {
//    std::cout << "***** input buffer, eventCount = " << eventCount << std::endl;
//    for(boost::circular_buffer<MidiEvent>::const_iterator iter = inputBuffer.begin(); iter != inputBuffer.end(); iter++) {
//        std::cout << "frame: " << (*iter).frameOffset() << std::endl;
//    }}

    // roll new events into ring buffer
    EventConnection *connection = eventInput.load();
    connection->process(rolling, pos, nframes, time);
    uint32_t numEvents = connection->getEventCount();
    for(uint32_t i = 0; i < numEvents; i++) {
        MidiEvent *evt = connection->getEvent(i);
        std::cout << "incoming event: " << *evt << std::endl;
        if(rolling) {
            int position = pos.tick + ((pos.beat - 1) * pos.ticks_per_beat);
            int division = pos.ticks_per_beat * pos.beats_per_bar;
            evt->setPosition(pos.bar, position + 1, division);
            inputBuffer.push_back(*evt);
        }
        // store control values
        if(evt->getType() == MidiEvent::TYPE_CONTROL) {
            controlValue[evt->getDatabyte1()].store(evt->getDatabyte2());
        }
    }

//    // try lock for critical section
//    int retcode = pthread_mutex_trylock(&activeMutex);
//    if(retcode) {
//        return;
//    }
//    // active request
//    if(clientWaiting){
//        long latencyOffset = 4410; // TODO: calculate latency from bitrate
//        // are we ready to return
//        if(end.frameOffset(pos) - latencyOffset < nframes) {
//            if(!controlRequest) { // event request
//                // copy events in range
//                eventCount = 0;
//                for(auto iter = inputBuffer.begin(); iter != inputBuffer.end(); iter++) {
//                    if(start <= iter->getStart() && iter->getStart() <= end) {
//                        // std::cout << "returning event in range: " << *iter << std::endl;
//                        outputBuffer[eventCount++] = (*iter);
//                    }
//                }
//            }
//            // reawaken client thread
//            this->clientWaiting = false;
//            int error = pthread_cond_signal(&inactiveCondition);
//        }
//    }
//    pthread_mutex_unlock(&activeMutex);
}
