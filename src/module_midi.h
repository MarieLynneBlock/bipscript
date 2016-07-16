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
#include <stdexcept>

class Note
{
    uint8_t vel;
public:
    int pitch;
    Duration duration;
    int channel;
    Note(int pitch, uint8_t velocity, int duration, int division)  :
        pitch(pitch), vel(velocity), duration(0, duration, division), channel(0) {}
    Note(int pitch, uint8_t velocity, Duration duration)  :
        pitch(pitch), vel(velocity), duration(duration), channel(0) {}
    Note(int pitch, uint8_t velocity, int channel) :
        pitch(pitch), vel(velocity), duration(0, 0, 1), channel(channel) {}
    unsigned int getPitch() {
        return pitch;
    }
    void transpose(int amount) {
        int value = amount + pitch;
        if(value < 0) {
            throw std::logic_error("Cannot transpose this note that low");
        }
        else if(value > 127) {
            throw std::logic_error("Cannot transpose this note that high");
        }
        pitch = value;
    }
    uint8_t velocity() const {
        return vel;
    }
    uint8_t velocity(uint8_t velocity) {
        if(velocity > 127) {
            throw std::logic_error("Velocity value cannot be greater than 127");
        }
        this->vel = velocity;
        return velocity;
    }
    void setDuration(const Duration &duration) {
        this->duration = duration;
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

class PitchBend
{
    uint32_t value;
public:
    PitchBend(uint32_t value) :
        value(value) {}
    uint32_t getValue() {
        return value;
    }
};

class ProgramChange
{
    uint8_t program;
public:
    ProgramChange(uint8_t program) :
        program(program) {}
    uint8_t getProgram() {
        return program;
    }
};

#endif // MIDIOUTPUT_H
