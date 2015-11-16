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
#ifndef MODULE_MIDI_H
#define MODULE_MIDI_H

#include "position.h"
#include <deque>

class Note
{
public:
    int note;
    int velocity;
    Duration duration;
    int channel;
    Note(int note, int velocity, int duration, int division)  :
        note(note), velocity(velocity), duration(0, duration, division), channel(0) {}
    Note(int note, int velocity, Duration duration)  :
        note(note), velocity(velocity), duration(duration), channel(0) {}
    unsigned int getPitch() {
        return note;
    }
};

class Control
{
public:
    int controller;
    int value;
    Control(int controller, int value) :
        controller(controller), value(value) {}
};

class ProgramChange
{
public:
    uint8_t program;
    ProgramChange(uint8_t program) :
        program(program) {}
};

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
    const Note &getNote() const {
        return note;
    }
};

class Pattern
{
    std::deque<PatternNote> noteList;
public:
    void addNote(Note &note, int bar, int position, int division);
    void addNote(Note &note, Position &position);
    void addNote(Note &note) { // TODO: expose when binding supports method overload
        addNote(note, 1, 1, 1);
    }
    unsigned int getSize() {
        return noteList.size();
    }
    const PatternNote &get(unsigned int index) {
        return noteList[index];
    }
    std::string print();
    //void schedule(MidiSink *sink, Position position);
};

#endif // MIDIOUTPUT_H
