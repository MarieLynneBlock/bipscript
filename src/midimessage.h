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
#ifndef MIDIMESSAGE_H
#define MIDIMESSAGE_H

class MidiMessage
{
public:
  virtual char type() = 0;
  virtual char byte(char index) = 0;
};

class Control : public MidiMessage
{
    int controller;
    int value;
public:
    Control(int controller, int value) :
        controller(controller), value(value) {}
    char type() { return 0xB0; }
    char byte(char index) {
      if(index == 0) { return controller; }
      return value;
    }
};

class PitchBend : public MidiMessage
{
    uint32_t value;
public:
    PitchBend(uint32_t value) : value(value) {}
    char type() { return 0xE0; }
    char byte(char index) {
      if(index == 0) { return value & 0x7F; }
      return value >> 7;
    }
};

class ProgramChange : public MidiMessage
{
    uint8_t program;
public:
    ProgramChange(uint8_t program) : program(program) {}
    char type() { return 0xC0; }
    char byte(char index) { return program; }
};

#endif // MIDIMESSAGE_H
