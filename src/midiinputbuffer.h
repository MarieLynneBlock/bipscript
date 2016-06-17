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
#ifndef MIDIINPUTBUFFER_H
#define MIDIINPUTBUFFER_H

#include "midiport.h"

#include <boost/circular_buffer.hpp>
#include <atomic>

class MidiInputBuffer : public Listable
{
    // buffers
    std::atomic<EventConnection *> eventInput;
    boost::circular_buffer<MidiEvent> inputBuffer;
    MidiEvent *outputBuffer;
    unsigned int eventCount;
    // request
    unsigned int controlRequest;
    Position start;
    Position end;
    // store control values
    std::atomic<unsigned char> controlValue[128];
    // condition variable to wait
//    bool clientWaiting;
//    pthread_mutex_t activeMutex;
//    pthread_cond_t inactiveCondition;
public:
    MidiInputBuffer(EventSource &source);
    ~MidiInputBuffer();
//    MidiEvent *getEvents(int bar, int position, int division, int *numEvents);
//    MidiEvent *getEvents(int bar, Position &start, Position &end, int *numEvents);
    unsigned int lastControlValue(int control);
    void processAll(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time);
    void reposition() {}
};

class MidiInputBufferCache : public ProcessorCache<MidiInputBuffer>
{
    MidiInputBuffer *createObject(EventSource *source) {
        return new MidiInputBuffer(*source);
    }
public:
    static MidiInputBufferCache &instance() {
        static MidiInputBufferCache instance;
        return instance;
    }
    MidiInputBuffer *getMidiInputBuffer(EventSource &source);
};

#endif // MIDIINPUTBUFFER_H
