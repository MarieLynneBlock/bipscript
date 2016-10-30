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
#ifndef MIDIPATTERN_H
#define MIDIPATTERN_H

#include "module_midi.h"
#include "eventlist.h"
#include "midievent.h"

class PatternNote {
    friend class Pattern;
    Note note;
    Position position;
public:
    PatternNote(Note &note, const Position &position)
        : note(note), position(position) {}
    const Position &getPosition() const {
        return position;
    }
    const Note &getNoteRef() const {
        return note;
    }
    Note *getNote() {
        return &note;
    }
};

class Pattern
{
    std::deque<PatternNote> noteList;
    EventList<MidiEvent> eventList;
public:
    // methods for notes
    void addNote(Note &note, int bar, int position, int division);
    void addNote(Note &note, Position &position);
    unsigned int size() {
        return noteList.size();
    }
    Note *getNote(uint32_t index);
    const PatternNote &get(unsigned int index) {
        return noteList[index];
    }
    // methods for events
    MidiEvent* getFirstEvent() {
        return eventList.getFirst();
    }
    MidiEvent* getNextEvent(MidiEvent *event) {
        return eventList.getNext(event);
    }
    std::string print();
    //void schedule(MidiSink *sink, Position position);
    void transpose(int amount);
};

#endif // MIDIPATTERN_H
