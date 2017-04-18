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
#ifndef MIDISINK_H
#define MIDISINK_H

#include "module_midi.h"
#include "midimessage.h"
#include "midipattern.h"
#include "midievent.h"

namespace bipscript {
namespace midi {

class Sink {
    unsigned char defaultChannel;
public:
    Sink() : defaultChannel(1) {}
    unsigned char midiChannel() {
        return this->defaultChannel;
    }
    unsigned char midiChannel(unsigned char channel) {
        return this->defaultChannel = channel;
    }
    void schedule(Note &note, unsigned int bar, unsigned int position, unsigned int division, unsigned char channel) {
        Position pos(bar, position, division);
        schedule(note, pos, channel);
    }
    void schedule(Note &note, unsigned int bar, unsigned int position, unsigned int division) {
        schedule(note, bar, position, division, defaultChannel);
    }
    void schedule(Note &note, unsigned int bar, unsigned int position) {
        schedule(note, bar, position, 4); // TODO: current time signature
    }
    void schedule(Note &note, unsigned int bar) {
        schedule(note, bar, 0);
    }
    void schedule(const Note &note, Position &position, unsigned char channel)
    {
        if(channel < 1 || channel > 16) {
            throw std::logic_error("MIDI channel must be between 1 and 16");
        }
        scheduleNote(note, position, channel);
    }
    void schedule(Pattern &pattern, unsigned int bar, unsigned int position, unsigned int division, unsigned char channel) {
        Position pos(bar, position, division);
        schedule(pattern, pos, channel);
    }
    void schedule(Pattern &pattern, unsigned int bar, unsigned int position, unsigned int division) {
        schedule(pattern, bar, position, division, defaultChannel);
    }
    void schedule(Pattern &pattern, unsigned int bar, unsigned int position) {
        schedule(pattern, bar, position, 4); // TODO: current time signature
    }
    void schedule(Pattern &pattern, unsigned int bar) {
        schedule(pattern, bar, 0);
    }
    void schedule(Pattern &pattern, Position &position, unsigned char channel);
    // Midi messages
    void schedule(Message &mesg, unsigned int bar, unsigned int position, unsigned int division, unsigned char channel) {
        Position pos(bar, position, division);
        schedule(mesg, pos, channel);
    }
    void schedule(Message &mesg, unsigned int bar, unsigned int position, unsigned int division) {
        schedule(mesg, bar, position, division, defaultChannel);
    }
    void schedule(Message &mesg, unsigned int bar, unsigned int position) {
        schedule(mesg, bar, position, 4); // TODO: current time signature
    }
    void schedule(Message &mesg, unsigned int bar) {
        schedule(mesg, bar, 0);
    }
    void schedule(Message &message, Position &position, unsigned char channel);
    virtual void addMidiEvent(Event* evt) = 0;
private:
    void scheduleNote(const Note &note, Position &position, unsigned char channel);
};

}}

#endif // MIDISINK_H
