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

#endif // MIDIOUTPUT_H
