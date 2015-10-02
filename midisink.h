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
#include "midievent.h"

class MidiSink {
    unsigned char defaultChannel;
public:
    MidiSink() : defaultChannel(1) {}
    void setDefaultChannel(unsigned char channel) {
        this->defaultChannel = channel;
    }
    void schedule(Control &cc, int bar, int position, int division);
    void schedule(Note &note, unsigned int bar, unsigned int position, unsigned int division, unsigned char channel) {
        Position pos(bar, position, division);
        schedule(note, pos, channel);
    }
    void schedule(Note &note, unsigned int bar, unsigned int position, unsigned int division) {
        schedule(note, bar, position, division, defaultChannel);
    }
    void schedule(const Note &note, Position &position, unsigned char channel);
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
        schedule(pattern, bar, 1);
    }
    void schedule(Pattern &pattern, Position &position, unsigned char channel);
    virtual void addMidiEvent(MidiEvent* evt) = 0;
};

#endif // MIDISINK_H
