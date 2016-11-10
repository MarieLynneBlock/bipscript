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
#ifndef BINDMIDI_H
#define BINDMIDI_H

#include "squirrel.h"
class Control;

namespace binding
{
    // object references to types in this package
    extern HSQOBJECT MidiABCReaderObject;
    extern HSQOBJECT MidiChordReaderObject;
    extern HSQOBJECT MidiDrumTabReaderObject;
    extern HSQOBJECT MidiSystemInObject;
    extern HSQOBJECT MidiNoteObject;
    extern HSQOBJECT MidiNoteOnObject;
    extern HSQOBJECT MidiNoteOffObject;
    extern HSQOBJECT MidiControlObject;
    extern HSQOBJECT MidiMMLReaderObject;
    extern HSQOBJECT MidiOutputObject;
    extern HSQOBJECT MidiPatternObject;
    extern HSQOBJECT MidiTuneObject;
    extern HSQOBJECT MidiSystemOutObject;
    extern HSQOBJECT MidiPitchBendObject;
    extern HSQOBJECT MidiProgramChangeObject;
    extern HSQOBJECT MidiBeatTrackerObject;
    SQInteger MidiControlPush(HSQUIRRELVM vm, Control *);
    // release hooks for types in this package
    SQInteger MidiABCReaderRelease(SQUserPointer p, SQInteger size);
    SQInteger MidiChordReaderRelease(SQUserPointer p, SQInteger size);
    SQInteger MidiDrumTabReaderRelease(SQUserPointer p, SQInteger size);
    SQInteger MidiNoteRelease(SQUserPointer p, SQInteger size);
    SQInteger MidiNoteOnRelease(SQUserPointer p, SQInteger size);
    SQInteger MidiNoteOffRelease(SQUserPointer p, SQInteger size);
    SQInteger MidiControlRelease(SQUserPointer p, SQInteger size);
    SQInteger MidiMMLReaderRelease(SQUserPointer p, SQInteger size);
    SQInteger MidiPatternRelease(SQUserPointer p, SQInteger size);
    SQInteger MidiTuneRelease(SQUserPointer p, SQInteger size);
    SQInteger MidiPitchBendRelease(SQUserPointer p, SQInteger size);
    SQInteger MidiProgramChangeRelease(SQUserPointer p, SQInteger size);
    // method to bind this package
    void bindMidi(HSQUIRRELVM vm);
}

#endif // BINDMIDI_H