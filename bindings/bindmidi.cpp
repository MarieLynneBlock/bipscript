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

#include "bindmidi.h"
#include "bindtypes.h"
#include "bindtransport.h"

#include "abcreader.h"
#include "chordreader.h"
#include "drumtabreader.h"
#include "midiport.h"
#include "module_midi.h"
#include "mmlreader.h"
#include "miditune.h"
#include "beattracker.h"

namespace binding {

// object references to types in this package
HSQOBJECT MidiABCReaderObject;
HSQOBJECT MidiChordReaderObject;
HSQOBJECT MidiDrumTabReaderObject;
HSQOBJECT MidiSystemInObject;
HSQOBJECT MidiNoteObject;
HSQOBJECT MidiNoteOnObject;
HSQOBJECT MidiNoteOffObject;
HSQOBJECT MidiControlObject;
HSQOBJECT MidiMMLReaderObject;
HSQOBJECT MidiOutputObject;
HSQOBJECT MidiPatternObject;
HSQOBJECT MidiTuneObject;
HSQOBJECT MidiSystemOutObject;
HSQOBJECT MidiPitchBendObject;
HSQOBJECT MidiProgramChangeObject;
HSQOBJECT MidiBeatTrackerObject;

//
// Midi.ABCReader class
//
SQInteger MidiABCReaderRelease(SQUserPointer p, SQInteger size)
{
    delete static_cast<ABCReader*>(p);
}

SQInteger MidiABCReaderCtor(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 1) {
        return sq_throwerror(vm, "too many parameters, expected at most 0");
    }
    ABCReader *obj;
    // call the implementation
    try {
        obj = new ABCReader();
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // return pointer to new object
    sq_setinstanceup(vm, 1, (SQUserPointer*)obj);
    sq_setreleasehook(vm, 1, MidiABCReaderRelease);
    return 1;
}

//
// Midi.ABCReader read
//
SQInteger MidiABCReaderread(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 6) {
        return sq_throwerror(vm, "too many parameters, expected at most 5");
    }
    if(numargs < 2) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 1");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "read method needs an instance of ABCReader");
    }
    ABCReader *obj = static_cast<ABCReader*>(userPtr);

    // get parameter 1 "abc" as string
    const SQChar* abc;
    if (SQ_FAILED(sq_getstring(vm, 2, &abc))){
        return sq_throwerror(vm, "argument 1 \"abc\" is not of type string");
    }

    // return value
    Pattern* ret;
    // 2 parameters passed in
    if(numargs == 3) {

        // get parameter 2 "key" as string
        const SQChar* key;
        if (SQ_FAILED(sq_getstring(vm, 3, &key))){
            return sq_throwerror(vm, "argument 2 \"key\" is not of type string");
        }

        // call the implementation
        try {
            ret = obj->read(abc, key);
        }
        catch(std::exception const& e) {
            return sq_throwerror(vm, e.what());
        }
    }

    // 3 parameters passed in
    else if(numargs == 4) {

        // get parameter 2 "key" as string
        const SQChar* key;
        if (SQ_FAILED(sq_getstring(vm, 3, &key))){
            return sq_throwerror(vm, "argument 2 \"key\" is not of type string");
        }

        // get parameter 3 "noteLength" as string
        const SQChar* noteLength;
        if (SQ_FAILED(sq_getstring(vm, 4, &noteLength))){
            return sq_throwerror(vm, "argument 3 \"noteLength\" is not of type string");
        }

        // call the implementation
        try {
            ret = obj->read(abc, key, noteLength);
        }
        catch(std::exception const& e) {
            return sq_throwerror(vm, e.what());
        }
    }

    // 4 parameters passed in
    else if(numargs == 5) {

        // get parameter 2 "key" as string
        const SQChar* key;
        if (SQ_FAILED(sq_getstring(vm, 3, &key))){
            return sq_throwerror(vm, "argument 2 \"key\" is not of type string");
        }

        // get parameter 3 "noteLength" as string
        const SQChar* noteLength;
        if (SQ_FAILED(sq_getstring(vm, 4, &noteLength))){
            return sq_throwerror(vm, "argument 3 \"noteLength\" is not of type string");
        }

        // get parameter 4 "meter" as string
        const SQChar* meter;
        if (SQ_FAILED(sq_getstring(vm, 5, &meter))){
            return sq_throwerror(vm, "argument 4 \"meter\" is not of type string");
        }

        // call the implementation
        try {
            ret = obj->read(abc, key, noteLength, meter);
        }
        catch(std::exception const& e) {
            return sq_throwerror(vm, e.what());
        }
    }

    // 5 parameters passed in
    else if(numargs == 6) {

        // get parameter 2 "key" as string
        const SQChar* key;
        if (SQ_FAILED(sq_getstring(vm, 3, &key))){
            return sq_throwerror(vm, "argument 2 \"key\" is not of type string");
        }

        // get parameter 3 "noteLength" as string
        const SQChar* noteLength;
        if (SQ_FAILED(sq_getstring(vm, 4, &noteLength))){
            return sq_throwerror(vm, "argument 3 \"noteLength\" is not of type string");
        }

        // get parameter 4 "meter" as string
        const SQChar* meter;
        if (SQ_FAILED(sq_getstring(vm, 5, &meter))){
            return sq_throwerror(vm, "argument 4 \"meter\" is not of type string");
        }

        // get parameter 5 "rhythm" as string
        const SQChar* rhythm;
        if (SQ_FAILED(sq_getstring(vm, 6, &rhythm))){
            return sq_throwerror(vm, "argument 5 \"rhythm\" is not of type string");
        }

        // call the implementation
        try {
            ret = obj->read(abc, key, noteLength, meter, rhythm);
        }
        catch(std::exception const& e) {
            return sq_throwerror(vm, e.what());
        }
    }

    else {
        // call the implementation
        try {
            ret = obj->read(abc);
        }
        catch(std::exception const& e) {
            return sq_throwerror(vm, e.what());
        }
    }

    // push return value
    sq_pushobject(vm, MidiPatternObject);
    sq_createinstance(vm, -1);
    sq_remove(vm, -2);
    sq_setinstanceup(vm, -1, ret);
    sq_setreleasehook(vm, -1, &MidiPatternRelease);

    return 1;
}

//
// Midi.ABCReader readTune
//
SQInteger MidiABCReaderreadTune(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 2) {
        return sq_throwerror(vm, "too many parameters, expected at most 1");
    }
    if(numargs < 2) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 1");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "readTune method needs an instance of ABCReader");
    }
    ABCReader *obj = static_cast<ABCReader*>(userPtr);

    // get parameter 1 "abc" as string
    const SQChar* abc;
    if (SQ_FAILED(sq_getstring(vm, 2, &abc))){
        return sq_throwerror(vm, "argument 1 \"abc\" is not of type string");
    }

    // return value
    MidiTune* ret;
    // call the implementation
    try {
        ret = obj->readTune(abc);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // push return value
    sq_pushobject(vm, MidiTuneObject);
    sq_createinstance(vm, -1);
    sq_remove(vm, -2);
    sq_setinstanceup(vm, -1, ret);
    sq_setreleasehook(vm, -1, &MidiTuneRelease);

    return 1;
}

//
// Midi.ChordReader class
//
SQInteger MidiChordReaderRelease(SQUserPointer p, SQInteger size)
{
    delete static_cast<ChordReader*>(p);
}

SQInteger MidiChordReaderCtor(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 1) {
        return sq_throwerror(vm, "too many parameters, expected at most 0");
    }
    ChordReader *obj;
    // call the implementation
    try {
        obj = new ChordReader();
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // return pointer to new object
    sq_setinstanceup(vm, 1, (SQUserPointer*)obj);
    sq_setreleasehook(vm, 1, MidiChordReaderRelease);
    return 1;
}

//
// Midi.ChordReader read
//
SQInteger MidiChordReaderread(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 4) {
        return sq_throwerror(vm, "too many parameters, expected at most 3");
    }
    if(numargs < 4) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 3");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "read method needs an instance of ChordReader");
    }
    ChordReader *obj = static_cast<ChordReader*>(userPtr);

    // get parameter 1 "chord" as string
    const SQChar* chord;
    if (SQ_FAILED(sq_getstring(vm, 2, &chord))){
        return sq_throwerror(vm, "argument 1 \"chord\" is not of type string");
    }

    // get parameter 2 "duration" as integer
    SQInteger duration;
    if (SQ_FAILED(sq_getinteger(vm, 3, &duration))){
        return sq_throwerror(vm, "argument 2 \"duration\" is not of type integer");
    }

    // get parameter 3 "division" as integer
    SQInteger division;
    if (SQ_FAILED(sq_getinteger(vm, 4, &division))){
        return sq_throwerror(vm, "argument 3 \"division\" is not of type integer");
    }

    // return value
    Pattern* ret;
    // call the implementation
    try {
        ret = obj->read(chord, duration, division);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // push return value
    sq_pushobject(vm, MidiPatternObject);
    sq_createinstance(vm, -1);
    sq_remove(vm, -2);
    sq_setinstanceup(vm, -1, ret);
    sq_setreleasehook(vm, -1, &MidiPatternRelease);

    return 1;
}

//
// Midi.DrumTabReader class
//
SQInteger MidiDrumTabReaderRelease(SQUserPointer p, SQInteger size)
{
    delete static_cast<DrumTabReader*>(p);
}

SQInteger MidiDrumTabReaderCtor(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 1) {
        return sq_throwerror(vm, "too many parameters, expected at most 0");
    }
    DrumTabReader *obj;
    // call the implementation
    try {
        obj = new DrumTabReader();
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // return pointer to new object
    sq_setinstanceup(vm, 1, (SQUserPointer*)obj);
    sq_setreleasehook(vm, 1, MidiDrumTabReaderRelease);
    return 1;
}

//
// Midi.DrumTabReader read
//
SQInteger MidiDrumTabReaderread(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 2) {
        return sq_throwerror(vm, "too many parameters, expected at most 1");
    }
    if(numargs < 2) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 1");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "read method needs an instance of DrumTabReader");
    }
    DrumTabReader *obj = static_cast<DrumTabReader*>(userPtr);

    // get parameter 1 "tab" as string
    const SQChar* tab;
    if (SQ_FAILED(sq_getstring(vm, 2, &tab))){
        return sq_throwerror(vm, "argument 1 \"tab\" is not of type string");
    }

    // return value
    Pattern* ret;
    // call the implementation
    try {
        ret = obj->read(tab);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // push return value
    sq_pushobject(vm, MidiPatternObject);
    sq_createinstance(vm, -1);
    sq_remove(vm, -2);
    sq_setinstanceup(vm, -1, ret);
    sq_setreleasehook(vm, -1, &MidiPatternRelease);

    return 1;
}

//
// Midi.DrumTabReader velocity
//
SQInteger MidiDrumTabReadervelocity(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 4) {
        return sq_throwerror(vm, "too many parameters, expected at most 3");
    }
    if(numargs < 4) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 3");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "velocity method needs an instance of DrumTabReader");
    }
    DrumTabReader *obj = static_cast<DrumTabReader*>(userPtr);

    // get parameter 1 "pitch" as integer
    SQInteger pitch;
    if (SQ_FAILED(sq_getinteger(vm, 2, &pitch))){
        return sq_throwerror(vm, "argument 1 \"pitch\" is not of type integer");
    }

    // get parameter 2 "code" as string
    const SQChar* code;
    if (SQ_FAILED(sq_getstring(vm, 3, &code))){
        return sq_throwerror(vm, "argument 2 \"code\" is not of type string");
    }

    // get parameter 3 "velocity" as integer
    SQInteger velocity;
    if (SQ_FAILED(sq_getinteger(vm, 4, &velocity))){
        return sq_throwerror(vm, "argument 3 \"velocity\" is not of type integer");
    }

    // call the implementation
    try {
        obj->setVelocity(pitch, code, velocity);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // void method, returns no value
    return 0;
}

//
// Midi.SystemIn class
//
SQInteger MidiSystemInCtor(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 3) {
        return sq_throwerror(vm, "too many parameters, expected at most 2");
    }
    if(numargs < 2) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 1");
    }
    // get parameter 1 "name" as string
    const SQChar* name;
    if (SQ_FAILED(sq_getstring(vm, 2, &name))){
        return sq_throwerror(vm, "argument 1 \"name\" is not of type string");
    }

    MidiInputPort *obj;
    // 2 parameters passed in
    if(numargs == 3) {

        // get parameter 2 "connection" as string
        const SQChar* connection;
        if (SQ_FAILED(sq_getstring(vm, 3, &connection))){
            return sq_throwerror(vm, "argument 2 \"connection\" is not of type string");
        }

        // call the implementation
        try {
            obj = MidiInputPortCache::instance().getMidiInputPort(name, connection);
        }
        catch(std::exception const& e) {
            return sq_throwerror(vm, e.what());
        }
    }

    else {
        // call the implementation
        try {
            obj = MidiInputPortCache::instance().getMidiInputPort(name);
        }
        catch(std::exception const& e) {
            return sq_throwerror(vm, e.what());
        }
    }

    // return pointer to new object
    sq_setinstanceup(vm, 1, (SQUserPointer*)obj);
    return 1;
}

//
// Midi.SystemIn midiOutput
//
SQInteger MidiSystemInmidiOutput(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 2) {
        return sq_throwerror(vm, "too many parameters, expected at most 1");
    }
    if(numargs < 2) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 1");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "midiOutput method needs an instance of SystemIn");
    }
    MidiInputPort *obj = static_cast<MidiInputPort*>(userPtr);

    // get parameter 1 "index" as integer
    SQInteger index;
    if (SQ_FAILED(sq_getinteger(vm, 2, &index))){
        return sq_throwerror(vm, "argument 1 \"index\" is not of type integer");
    }

    // return value
    MidiConnection* ret;
    // call the implementation
    try {
        ret = obj->getMidiConnection(index);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // push return value
    sq_pushobject(vm, MidiOutputObject);
    sq_createinstance(vm, -1);
    sq_remove(vm, -2);
    sq_setinstanceup(vm, -1, ret);
    // no release hook, release ignored per binding

    return 1;
}

//
// Midi.SystemIn onControl
//
SQInteger MidiSystemInonControl(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 2) {
        return sq_throwerror(vm, "too many parameters, expected at most 1");
    }
    if(numargs < 2) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 1");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "onControl method needs an instance of SystemIn");
    }
    MidiInputPort *obj = static_cast<MidiInputPort*>(userPtr);

    // get parameter 1 "handler" as function
    HSQOBJECT handlerObj;
    if (SQ_FAILED(sq_getstackobj(vm, 2, &handlerObj))) {
        return sq_throwerror(vm, "argument 1 \"handler\" is not of type function");
    }
    if (sq_gettype(vm, 2) != OT_CLOSURE) {
        return sq_throwerror(vm, "argument 1 \"handler\" is not of type function");
    }
    SQUnsignedInteger nparams, nfreevars;
    sq_getclosureinfo(vm, 2, &nparams, &nfreevars);
    sq_addref(vm, &handlerObj);
    ScriptFunction handler(vm, handlerObj, nparams);

    // call the implementation
    try {
        obj->onControl(handler);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // void method, returns no value
    return 0;
}

//
// Midi.SystemIn onNoteOff
//
SQInteger MidiSystemInonNoteOff(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 2) {
        return sq_throwerror(vm, "too many parameters, expected at most 1");
    }
    if(numargs < 2) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 1");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "onNoteOff method needs an instance of SystemIn");
    }
    MidiInputPort *obj = static_cast<MidiInputPort*>(userPtr);

    // get parameter 1 "handler" as function
    HSQOBJECT handlerObj;
    if (SQ_FAILED(sq_getstackobj(vm, 2, &handlerObj))) {
        return sq_throwerror(vm, "argument 1 \"handler\" is not of type function");
    }
    if (sq_gettype(vm, 2) != OT_CLOSURE) {
        return sq_throwerror(vm, "argument 1 \"handler\" is not of type function");
    }
    SQUnsignedInteger nparams, nfreevars;
    sq_getclosureinfo(vm, 2, &nparams, &nfreevars);
    sq_addref(vm, &handlerObj);
    ScriptFunction handler(vm, handlerObj, nparams);

    // call the implementation
    try {
        obj->onNoteOff(handler);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // void method, returns no value
    return 0;
}

//
// Midi.SystemIn onNoteOn
//
SQInteger MidiSystemInonNoteOn(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 2) {
        return sq_throwerror(vm, "too many parameters, expected at most 1");
    }
    if(numargs < 2) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 1");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "onNoteOn method needs an instance of SystemIn");
    }
    MidiInputPort *obj = static_cast<MidiInputPort*>(userPtr);

    // get parameter 1 "handler" as function
    HSQOBJECT handlerObj;
    if (SQ_FAILED(sq_getstackobj(vm, 2, &handlerObj))) {
        return sq_throwerror(vm, "argument 1 \"handler\" is not of type function");
    }
    if (sq_gettype(vm, 2) != OT_CLOSURE) {
        return sq_throwerror(vm, "argument 1 \"handler\" is not of type function");
    }
    SQUnsignedInteger nparams, nfreevars;
    sq_getclosureinfo(vm, 2, &nparams, &nfreevars);
    sq_addref(vm, &handlerObj);
    ScriptFunction handler(vm, handlerObj, nparams);

    // call the implementation
    try {
        obj->onNoteOn(handler);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // void method, returns no value
    return 0;
}

//
// Midi.Note class
//
SQInteger MidiNoteRelease(SQUserPointer p, SQInteger size)
{
    delete static_cast<Note*>(p);
}

SQInteger MidiNoteCtor(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 5) {
        return sq_throwerror(vm, "too many parameters, expected at most 4");
    }
    if(numargs < 5) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 4");
    }
    // get parameter 1 "pitch" as integer
    SQInteger pitch;
    if (SQ_FAILED(sq_getinteger(vm, 2, &pitch))){
        return sq_throwerror(vm, "argument 1 \"pitch\" is not of type integer");
    }

    // get parameter 2 "velocity" as integer
    SQInteger velocity;
    if (SQ_FAILED(sq_getinteger(vm, 3, &velocity))){
        return sq_throwerror(vm, "argument 2 \"velocity\" is not of type integer");
    }

    // get parameter 3 "duration" as integer
    SQInteger duration;
    if (SQ_FAILED(sq_getinteger(vm, 4, &duration))){
        return sq_throwerror(vm, "argument 3 \"duration\" is not of type integer");
    }

    // get parameter 4 "division" as integer
    SQInteger division;
    if (SQ_FAILED(sq_getinteger(vm, 5, &division))){
        return sq_throwerror(vm, "argument 4 \"division\" is not of type integer");
    }

    Note *obj;
    // call the implementation
    try {
        obj = new Note(pitch, velocity, duration, division);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // return pointer to new object
    sq_setinstanceup(vm, 1, (SQUserPointer*)obj);
    sq_setreleasehook(vm, 1, MidiNoteRelease);
    return 1;
}

//
// Midi.Note pitch
//
SQInteger MidiNotepitch(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 1) {
        return sq_throwerror(vm, "too many parameters, expected at most 0");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "pitch method needs an instance of Note");
    }
    Note *obj = static_cast<Note*>(userPtr);

    // return value
    SQInteger ret;
    // call the implementation
    try {
        ret = obj->pitch();
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // push return value
    sq_pushinteger(vm, ret);
    return 1;
}

//
// Midi.Note transpose
//
SQInteger MidiNotetranspose(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 2) {
        return sq_throwerror(vm, "too many parameters, expected at most 1");
    }
    if(numargs < 2) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 1");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "transpose method needs an instance of Note");
    }
    Note *obj = static_cast<Note*>(userPtr);

    // get parameter 1 "amount" as integer
    SQInteger amount;
    if (SQ_FAILED(sq_getinteger(vm, 2, &amount))){
        return sq_throwerror(vm, "argument 1 \"amount\" is not of type integer");
    }

    // call the implementation
    try {
        obj->transpose(amount);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // void method, returns no value
    return 0;
}

//
// Midi.Note velocity
//
SQInteger MidiNotevelocity(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 2) {
        return sq_throwerror(vm, "too many parameters, expected at most 1");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "velocity method needs an instance of Note");
    }
    Note *obj = static_cast<Note*>(userPtr);

    // return value
    SQInteger ret;
    // 1 parameters passed in
    if(numargs == 2) {

        // get parameter 1 "value" as integer
        SQInteger value;
        if (SQ_FAILED(sq_getinteger(vm, 2, &value))){
            return sq_throwerror(vm, "argument 1 \"value\" is not of type integer");
        }

        // call the implementation
        try {
            ret = obj->velocity(value);
        }
        catch(std::exception const& e) {
            return sq_throwerror(vm, e.what());
        }
    }

    else {
        // call the implementation
        try {
            ret = obj->velocity();
        }
        catch(std::exception const& e) {
            return sq_throwerror(vm, e.what());
        }
    }

    // push return value
    sq_pushinteger(vm, ret);
    return 1;
}

//
// Midi.NoteOn class
//
SQInteger MidiNoteOnPush(HSQUIRRELVM vm, NoteOn *obj)
{
    sq_pushobject(vm, MidiNoteOnObject);
    sq_createinstance(vm, -1);
    sq_remove(vm, -2);
    sq_setinstanceup(vm, -1, new NoteOn(*obj));
    sq_setreleasehook(vm, -1, &MidiNoteOnRelease);
}

SQInteger MidiNoteOnRelease(SQUserPointer p, SQInteger size)
{
    delete static_cast<NoteOn*>(p);
}

SQInteger MidiNoteOnCtor(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 3) {
        return sq_throwerror(vm, "too many parameters, expected at most 2");
    }
    if(numargs < 3) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 2");
    }
    // get parameter 1 "pitch" as integer
    SQInteger pitch;
    if (SQ_FAILED(sq_getinteger(vm, 2, &pitch))){
        return sq_throwerror(vm, "argument 1 \"pitch\" is not of type integer");
    }

    // get parameter 2 "velocity" as integer
    SQInteger velocity;
    if (SQ_FAILED(sq_getinteger(vm, 3, &velocity))){
        return sq_throwerror(vm, "argument 2 \"velocity\" is not of type integer");
    }

    NoteOn *obj;
    // call the implementation
    try {
        obj = new NoteOn(pitch, velocity);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // return pointer to new object
    sq_setinstanceup(vm, 1, (SQUserPointer*)obj);
    sq_setreleasehook(vm, 1, MidiNoteOnRelease);
    return 1;
}

//
// Midi.NoteOn pitch
//
SQInteger MidiNoteOnpitch(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 1) {
        return sq_throwerror(vm, "too many parameters, expected at most 0");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "pitch method needs an instance of NoteOn");
    }
    NoteOn *obj = static_cast<NoteOn*>(userPtr);

    // return value
    SQInteger ret;
    // call the implementation
    try {
        ret = obj->getPitch();
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // push return value
    sq_pushinteger(vm, ret);
    return 1;
}

//
// Midi.NoteOn velocity
//
SQInteger MidiNoteOnvelocity(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 1) {
        return sq_throwerror(vm, "too many parameters, expected at most 0");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "velocity method needs an instance of NoteOn");
    }
    NoteOn *obj = static_cast<NoteOn*>(userPtr);

    // return value
    SQInteger ret;
    // call the implementation
    try {
        ret = obj->getVelocity();
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // push return value
    sq_pushinteger(vm, ret);
    return 1;
}

//
// Midi.NoteOff class
//
SQInteger MidiNoteOffPush(HSQUIRRELVM vm, NoteOff *obj)
{
    sq_pushobject(vm, MidiNoteOffObject);
    sq_createinstance(vm, -1);
    sq_remove(vm, -2);
    sq_setinstanceup(vm, -1, new NoteOff(*obj));
    sq_setreleasehook(vm, -1, &MidiNoteOffRelease);
}

SQInteger MidiNoteOffRelease(SQUserPointer p, SQInteger size)
{
    delete static_cast<NoteOff*>(p);
}

SQInteger MidiNoteOffCtor(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 3) {
        return sq_throwerror(vm, "too many parameters, expected at most 2");
    }
    if(numargs < 2) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 1");
    }
    // get parameter 1 "pitch" as integer
    SQInteger pitch;
    if (SQ_FAILED(sq_getinteger(vm, 2, &pitch))){
        return sq_throwerror(vm, "argument 1 \"pitch\" is not of type integer");
    }

    NoteOff *obj;
    // 2 parameters passed in
    if(numargs == 3) {

        // get parameter 2 "velocity" as integer
        SQInteger velocity;
        if (SQ_FAILED(sq_getinteger(vm, 3, &velocity))){
            return sq_throwerror(vm, "argument 2 \"velocity\" is not of type integer");
        }

        // call the implementation
        try {
            obj = new NoteOff(pitch, velocity);
        }
        catch(std::exception const& e) {
            return sq_throwerror(vm, e.what());
        }
    }

    else {
        // call the implementation
        try {
            obj = new NoteOff(pitch);
        }
        catch(std::exception const& e) {
            return sq_throwerror(vm, e.what());
        }
    }

    // return pointer to new object
    sq_setinstanceup(vm, 1, (SQUserPointer*)obj);
    sq_setreleasehook(vm, 1, MidiNoteOffRelease);
    return 1;
}

//
// Midi.NoteOff pitch
//
SQInteger MidiNoteOffpitch(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 1) {
        return sq_throwerror(vm, "too many parameters, expected at most 0");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "pitch method needs an instance of NoteOff");
    }
    NoteOff *obj = static_cast<NoteOff*>(userPtr);

    // return value
    SQInteger ret;
    // call the implementation
    try {
        ret = obj->getPitch();
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // push return value
    sq_pushinteger(vm, ret);
    return 1;
}

//
// Midi.NoteOff velocity
//
SQInteger MidiNoteOffvelocity(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 1) {
        return sq_throwerror(vm, "too many parameters, expected at most 0");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "velocity method needs an instance of NoteOff");
    }
    NoteOff *obj = static_cast<NoteOff*>(userPtr);

    // return value
    SQInteger ret;
    // call the implementation
    try {
        ret = obj->getVelocity();
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // push return value
    sq_pushinteger(vm, ret);
    return 1;
}

//
// Midi.Control class
//
SQInteger MidiControlPush(HSQUIRRELVM vm, Control *obj)
{
    sq_pushobject(vm, MidiControlObject);
    sq_createinstance(vm, -1);
    sq_remove(vm, -2);
    sq_setinstanceup(vm, -1, new Control(*obj));
    sq_setreleasehook(vm, -1, &MidiControlRelease);
}

SQInteger MidiControlRelease(SQUserPointer p, SQInteger size)
{
    delete static_cast<Control*>(p);
}

SQInteger MidiControlCtor(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 3) {
        return sq_throwerror(vm, "too many parameters, expected at most 2");
    }
    if(numargs < 3) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 2");
    }
    // get parameter 1 "control" as integer
    SQInteger control;
    if (SQ_FAILED(sq_getinteger(vm, 2, &control))){
        return sq_throwerror(vm, "argument 1 \"control\" is not of type integer");
    }

    // get parameter 2 "value" as integer
    SQInteger value;
    if (SQ_FAILED(sq_getinteger(vm, 3, &value))){
        return sq_throwerror(vm, "argument 2 \"value\" is not of type integer");
    }

    Control *obj;
    // call the implementation
    try {
        obj = new Control(control, value);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // return pointer to new object
    sq_setinstanceup(vm, 1, (SQUserPointer*)obj);
    sq_setreleasehook(vm, 1, MidiControlRelease);
    return 1;
}

//
// Midi.Control controller
//
SQInteger MidiControlcontroller(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 1) {
        return sq_throwerror(vm, "too many parameters, expected at most 0");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "controller method needs an instance of Control");
    }
    Control *obj = static_cast<Control*>(userPtr);

    // return value
    SQInteger ret;
    // call the implementation
    try {
        ret = obj->getController();
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // push return value
    sq_pushinteger(vm, ret);
    return 1;
}

//
// Midi.Control value
//
SQInteger MidiControlvalue(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 1) {
        return sq_throwerror(vm, "too many parameters, expected at most 0");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "value method needs an instance of Control");
    }
    Control *obj = static_cast<Control*>(userPtr);

    // return value
    SQInteger ret;
    // call the implementation
    try {
        ret = obj->getValue();
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // push return value
    sq_pushinteger(vm, ret);
    return 1;
}

//
// Midi.MMLReader class
//
SQInteger MidiMMLReaderRelease(SQUserPointer p, SQInteger size)
{
    delete static_cast<MMLReader*>(p);
}

SQInteger MidiMMLReaderCtor(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 1) {
        return sq_throwerror(vm, "too many parameters, expected at most 0");
    }
    MMLReader *obj;
    // call the implementation
    try {
        obj = new MMLReader();
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // return pointer to new object
    sq_setinstanceup(vm, 1, (SQUserPointer*)obj);
    sq_setreleasehook(vm, 1, MidiMMLReaderRelease);
    return 1;
}

//
// Midi.MMLReader read
//
SQInteger MidiMMLReaderread(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 2) {
        return sq_throwerror(vm, "too many parameters, expected at most 1");
    }
    if(numargs < 2) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 1");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "read method needs an instance of MMLReader");
    }
    MMLReader *obj = static_cast<MMLReader*>(userPtr);

    // get parameter 1 "mml" as string
    const SQChar* mml;
    if (SQ_FAILED(sq_getstring(vm, 2, &mml))){
        return sq_throwerror(vm, "argument 1 \"mml\" is not of type string");
    }

    // return value
    Pattern* ret;
    // call the implementation
    try {
        ret = obj->read(mml);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // push return value
    sq_pushobject(vm, MidiPatternObject);
    sq_createinstance(vm, -1);
    sq_remove(vm, -2);
    sq_setinstanceup(vm, -1, ret);
    sq_setreleasehook(vm, -1, &MidiPatternRelease);

    return 1;
}

//
// Midi.Output class
//
SQInteger MidiOutputCtor(HSQUIRRELVM vm)
{
    return sq_throwerror(vm, "cannot directly instantiate Midi.Output");
}

//
// Midi.Output onControl
//
SQInteger MidiOutputonControl(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 2) {
        return sq_throwerror(vm, "too many parameters, expected at most 1");
    }
    if(numargs < 2) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 1");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "onControl method needs an instance of Output");
    }
    MidiConnection *obj = static_cast<MidiConnection*>(userPtr);

    // get parameter 1 "handler" as function
    HSQOBJECT handlerObj;
    if (SQ_FAILED(sq_getstackobj(vm, 2, &handlerObj))) {
        return sq_throwerror(vm, "argument 1 \"handler\" is not of type function");
    }
    if (sq_gettype(vm, 2) != OT_CLOSURE) {
        return sq_throwerror(vm, "argument 1 \"handler\" is not of type function");
    }
    SQUnsignedInteger nparams, nfreevars;
    sq_getclosureinfo(vm, 2, &nparams, &nfreevars);
    sq_addref(vm, &handlerObj);
    ScriptFunction handler(vm, handlerObj, nparams);

    // call the implementation
    try {
        obj->onControl(handler);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // void method, returns no value
    return 0;
}

//
// Midi.Output onNoteOff
//
SQInteger MidiOutputonNoteOff(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 2) {
        return sq_throwerror(vm, "too many parameters, expected at most 1");
    }
    if(numargs < 2) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 1");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "onNoteOff method needs an instance of Output");
    }
    MidiConnection *obj = static_cast<MidiConnection*>(userPtr);

    // get parameter 1 "handler" as function
    HSQOBJECT handlerObj;
    if (SQ_FAILED(sq_getstackobj(vm, 2, &handlerObj))) {
        return sq_throwerror(vm, "argument 1 \"handler\" is not of type function");
    }
    if (sq_gettype(vm, 2) != OT_CLOSURE) {
        return sq_throwerror(vm, "argument 1 \"handler\" is not of type function");
    }
    SQUnsignedInteger nparams, nfreevars;
    sq_getclosureinfo(vm, 2, &nparams, &nfreevars);
    sq_addref(vm, &handlerObj);
    ScriptFunction handler(vm, handlerObj, nparams);

    // call the implementation
    try {
        obj->onNoteOff(handler);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // void method, returns no value
    return 0;
}

//
// Midi.Output onNoteOn
//
SQInteger MidiOutputonNoteOn(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 2) {
        return sq_throwerror(vm, "too many parameters, expected at most 1");
    }
    if(numargs < 2) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 1");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "onNoteOn method needs an instance of Output");
    }
    MidiConnection *obj = static_cast<MidiConnection*>(userPtr);

    // get parameter 1 "handler" as function
    HSQOBJECT handlerObj;
    if (SQ_FAILED(sq_getstackobj(vm, 2, &handlerObj))) {
        return sq_throwerror(vm, "argument 1 \"handler\" is not of type function");
    }
    if (sq_gettype(vm, 2) != OT_CLOSURE) {
        return sq_throwerror(vm, "argument 1 \"handler\" is not of type function");
    }
    SQUnsignedInteger nparams, nfreevars;
    sq_getclosureinfo(vm, 2, &nparams, &nfreevars);
    sq_addref(vm, &handlerObj);
    ScriptFunction handler(vm, handlerObj, nparams);

    // call the implementation
    try {
        obj->onNoteOn(handler);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // void method, returns no value
    return 0;
}

//
// Midi.Pattern class
//
SQInteger MidiPatternRelease(SQUserPointer p, SQInteger size)
{
    delete static_cast<Pattern*>(p);
}

SQInteger MidiPatternCtor(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 2) {
        return sq_throwerror(vm, "too many parameters, expected at most 1");
    }
    Pattern *obj;
    // 1 parameters passed in
    if(numargs == 2) {

        // get parameter 1 "other" as Midi.Pattern
        Pattern *other = 0;
        sq_getinstanceup(vm, 2, (SQUserPointer*)&other, 0);
        if(other == 0) {
            return sq_throwerror(vm, "argument 1 \"other\" is not of type Midi.Pattern");
        }
        SQUserPointer otherTypeTag;
        sq_gettypetag(vm, 2, &otherTypeTag);
        if(otherTypeTag != &MidiPatternObject) {
            return sq_throwerror(vm, "argument 1 \"other\" is not of type Midi.Pattern");
        }

        // call the implementation
        try {
            obj = new Pattern(*other);
        }
        catch(std::exception const& e) {
            return sq_throwerror(vm, e.what());
        }
    }

    else {
        // call the implementation
        try {
            obj = new Pattern();
        }
        catch(std::exception const& e) {
            return sq_throwerror(vm, e.what());
        }
    }

    // return pointer to new object
    sq_setinstanceup(vm, 1, (SQUserPointer*)obj);
    sq_setreleasehook(vm, 1, MidiPatternRelease);
    return 1;
}

//
// Midi.Pattern add
//
SQInteger MidiPatternadd(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 5) {
        return sq_throwerror(vm, "too many parameters, expected at most 4");
    }
    if(numargs < 5) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 4");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "add method needs an instance of Pattern");
    }
    Pattern *obj = static_cast<Pattern*>(userPtr);

    // get parameter 1 "note" as Midi.Note
    Note *note = 0;
    sq_getinstanceup(vm, 2, (SQUserPointer*)&note, 0);
    if(note == 0) {
        return sq_throwerror(vm, "argument 1 \"note\" is not of type Midi.Note");
    }
    SQUserPointer noteTypeTag;
    sq_gettypetag(vm, 2, &noteTypeTag);
    if(noteTypeTag != &MidiNoteObject) {
        return sq_throwerror(vm, "argument 1 \"note\" is not of type Midi.Note");
    }

    // get parameter 2 "bar" as integer
    SQInteger bar;
    if (SQ_FAILED(sq_getinteger(vm, 3, &bar))){
        return sq_throwerror(vm, "argument 2 \"bar\" is not of type integer");
    }

    // get parameter 3 "position" as integer
    SQInteger position;
    if (SQ_FAILED(sq_getinteger(vm, 4, &position))){
        return sq_throwerror(vm, "argument 3 \"position\" is not of type integer");
    }

    // get parameter 4 "division" as integer
    SQInteger division;
    if (SQ_FAILED(sq_getinteger(vm, 5, &division))){
        return sq_throwerror(vm, "argument 4 \"division\" is not of type integer");
    }

    // call the implementation
    try {
        obj->addNote(*note, bar, position, division);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // void method, returns no value
    return 0;
}

//
// Midi.Pattern note
//
SQInteger MidiPatternnote(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 2) {
        return sq_throwerror(vm, "too many parameters, expected at most 1");
    }
    if(numargs < 2) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 1");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "note method needs an instance of Pattern");
    }
    Pattern *obj = static_cast<Pattern*>(userPtr);

    // get parameter 1 "index" as integer
    SQInteger index;
    if (SQ_FAILED(sq_getinteger(vm, 2, &index))){
        return sq_throwerror(vm, "argument 1 \"index\" is not of type integer");
    }

    // return value
    Note* ret;
    // call the implementation
    try {
        ret = obj->getNote(index);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // push return value
    sq_pushobject(vm, MidiNoteObject);
    sq_createinstance(vm, -1);
    sq_remove(vm, -2);
    sq_setinstanceup(vm, -1, ret);
    // no release hook, release ignored per binding

    return 1;
}

//
// Midi.Pattern print
//
SQInteger MidiPatternprint(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 1) {
        return sq_throwerror(vm, "too many parameters, expected at most 0");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "print method needs an instance of Pattern");
    }
    Pattern *obj = static_cast<Pattern*>(userPtr);

    // call the implementation
    try {
        obj->print();
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // void method, returns no value
    return 0;
}

//
// Midi.Pattern size
//
SQInteger MidiPatternsize(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 1) {
        return sq_throwerror(vm, "too many parameters, expected at most 0");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "size method needs an instance of Pattern");
    }
    Pattern *obj = static_cast<Pattern*>(userPtr);

    // return value
    SQInteger ret;
    // call the implementation
    try {
        ret = obj->size();
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // push return value
    sq_pushinteger(vm, ret);
    return 1;
}

//
// Midi.Pattern transpose
//
SQInteger MidiPatterntranspose(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 2) {
        return sq_throwerror(vm, "too many parameters, expected at most 1");
    }
    if(numargs < 2) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 1");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "transpose method needs an instance of Pattern");
    }
    Pattern *obj = static_cast<Pattern*>(userPtr);

    // get parameter 1 "amount" as integer
    SQInteger amount;
    if (SQ_FAILED(sq_getinteger(vm, 2, &amount))){
        return sq_throwerror(vm, "argument 1 \"amount\" is not of type integer");
    }

    // call the implementation
    try {
        obj->transpose(amount);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // void method, returns no value
    return 0;
}

//
// Midi.Tune class
//
SQInteger MidiTuneRelease(SQUserPointer p, SQInteger size)
{
    delete static_cast<MidiTune*>(p);
}

SQInteger MidiTuneCtor(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 2) {
        return sq_throwerror(vm, "too many parameters, expected at most 1");
    }
    if(numargs < 2) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 1");
    }
    // get parameter 1 "tracks" as integer
    SQInteger tracks;
    if (SQ_FAILED(sq_getinteger(vm, 2, &tracks))){
        return sq_throwerror(vm, "argument 1 \"tracks\" is not of type integer");
    }

    MidiTune *obj;
    // call the implementation
    try {
        obj = new MidiTune(tracks);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // return pointer to new object
    sq_setinstanceup(vm, 1, (SQUserPointer*)obj);
    sq_setreleasehook(vm, 1, MidiTuneRelease);
    return 1;
}

//
// Midi.Tune timeSignature
//
SQInteger MidiTunetimeSignature(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 1) {
        return sq_throwerror(vm, "too many parameters, expected at most 0");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "timeSignature method needs an instance of Tune");
    }
    MidiTune *obj = static_cast<MidiTune*>(userPtr);

    // return value
    TimeSignature* ret;
    // call the implementation
    try {
        ret = obj->getTimeSignature();
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // push return value
    sq_pushobject(vm, TransportTimeSignatureObject);
    sq_createinstance(vm, -1);
    sq_remove(vm, -2);
    sq_setinstanceup(vm, -1, ret);
    sq_setreleasehook(vm, -1, &TransportTimeSignatureRelease);

    return 1;
}

//
// Midi.Tune title
//
SQInteger MidiTunetitle(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 1) {
        return sq_throwerror(vm, "too many parameters, expected at most 0");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "title method needs an instance of Tune");
    }
    MidiTune *obj = static_cast<MidiTune*>(userPtr);

    // return value
    const SQChar* ret;
    // call the implementation
    try {
        ret = obj->getTitle();
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // push return value
    sq_pushstring(vm, ret, strlen(ret));
    return 1;
}

//
// Midi.Tune track
//
SQInteger MidiTunetrack(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 2) {
        return sq_throwerror(vm, "too many parameters, expected at most 1");
    }
    if(numargs < 2) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 1");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "track method needs an instance of Tune");
    }
    MidiTune *obj = static_cast<MidiTune*>(userPtr);

    // get parameter 1 "number" as integer
    SQInteger number;
    if (SQ_FAILED(sq_getinteger(vm, 2, &number))){
        return sq_throwerror(vm, "argument 1 \"number\" is not of type integer");
    }

    // return value
    Pattern* ret;
    // call the implementation
    try {
        ret = obj->track(number);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // push return value
    sq_pushobject(vm, MidiPatternObject);
    sq_createinstance(vm, -1);
    sq_remove(vm, -2);
    sq_setinstanceup(vm, -1, ret);
    // no release hook, release ignored per binding

    return 1;
}

//
// Midi.Tune trackCount
//
SQInteger MidiTunetrackCount(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 1) {
        return sq_throwerror(vm, "too many parameters, expected at most 0");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "trackCount method needs an instance of Tune");
    }
    MidiTune *obj = static_cast<MidiTune*>(userPtr);

    // return value
    SQInteger ret;
    // call the implementation
    try {
        ret = obj->trackCount();
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // push return value
    sq_pushinteger(vm, ret);
    return 1;
}

//
// Midi.SystemOut class
//
SQInteger MidiSystemOutCtor(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 3) {
        return sq_throwerror(vm, "too many parameters, expected at most 2");
    }
    if(numargs < 2) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 1");
    }
    // get parameter 1 "name" as string
    const SQChar* name;
    if (SQ_FAILED(sq_getstring(vm, 2, &name))){
        return sq_throwerror(vm, "argument 1 \"name\" is not of type string");
    }

    MidiOutputPort *obj;
    // 2 parameters passed in
    if(numargs == 3) {

        // get parameter 2 "connection" as string
        const SQChar* connection;
        if (SQ_FAILED(sq_getstring(vm, 3, &connection))){
            return sq_throwerror(vm, "argument 2 \"connection\" is not of type string");
        }

        // call the implementation
        try {
            obj = MidiOutputPortCache::instance().getMidiOutputPort(name, connection);
        }
        catch(std::exception const& e) {
            return sq_throwerror(vm, e.what());
        }
    }

    else {
        // call the implementation
        try {
            obj = MidiOutputPortCache::instance().getMidiOutputPort(name);
        }
        catch(std::exception const& e) {
            return sq_throwerror(vm, e.what());
        }
    }

    // return pointer to new object
    sq_setinstanceup(vm, 1, (SQUserPointer*)obj);
    return 1;
}

//
// Midi.SystemOut midiChannel
//
SQInteger MidiSystemOutmidiChannel(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 2) {
        return sq_throwerror(vm, "too many parameters, expected at most 1");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "midiChannel method needs an instance of SystemOut");
    }
    MidiOutputPort *obj = static_cast<MidiOutputPort*>(userPtr);

    // return value
    SQInteger ret;
    // 1 parameters passed in
    if(numargs == 2) {

        // get parameter 1 "channel" as integer
        SQInteger channel;
        if (SQ_FAILED(sq_getinteger(vm, 2, &channel))){
            return sq_throwerror(vm, "argument 1 \"channel\" is not of type integer");
        }

        // call the implementation
        try {
            ret = obj->midiChannel(channel);
        }
        catch(std::exception const& e) {
            return sq_throwerror(vm, e.what());
        }
    }

    else {
        // call the implementation
        try {
            ret = obj->midiChannel();
        }
        catch(std::exception const& e) {
            return sq_throwerror(vm, e.what());
        }
    }

    // push return value
    sq_pushinteger(vm, ret);
    return 1;
}

//
// Midi.SystemOut schedule
//
SQInteger MidiSystemOutschedule(HSQUIRRELVM vm)
{
    SQObjectType overrideType = sq_gettype(vm, 2);
    SQUserPointer overrideTypeTag;
    if(overrideType == OT_INSTANCE) {
        sq_gettypetag(vm, 2, &overrideTypeTag);
    }

    if(overrideType == OT_INSTANCE && overrideTypeTag == &MidiNoteObject) {
        SQInteger numargs = sq_gettop(vm);
        // check parameter count
        if(numargs > 6) {
            return sq_throwerror(vm, "too many parameters, expected at most 5");
        }
        if(numargs < 3) {
            return sq_throwerror(vm, "insufficient parameters, expected at least 2");
        }
        // get "this" pointer
        SQUserPointer userPtr = 0;
        if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
            return sq_throwerror(vm, "schedule method needs an instance of SystemOut");
        }
        MidiOutputPort *obj = static_cast<MidiOutputPort*>(userPtr);

        // get parameter 1 "note" as Midi.Note
        Note *note = 0;
        sq_getinstanceup(vm, 2, (SQUserPointer*)&note, 0);
        if(note == 0) {
            return sq_throwerror(vm, "argument 1 \"note\" is not of type Midi.Note");
        }

        // get parameter 2 "bar" as integer
        SQInteger bar;
        if (SQ_FAILED(sq_getinteger(vm, 3, &bar))){
            return sq_throwerror(vm, "argument 2 \"bar\" is not of type integer");
        }

        // 3 parameters passed in
        if(numargs == 4) {

            // get parameter 3 "position" as integer
            SQInteger position;
            if (SQ_FAILED(sq_getinteger(vm, 4, &position))){
                return sq_throwerror(vm, "argument 3 \"position\" is not of type integer");
            }

            // call the implementation
            try {
                obj->schedule(*note, bar, position);
            }
            catch(std::exception const& e) {
                return sq_throwerror(vm, e.what());
            }
        }

        // 4 parameters passed in
        else if(numargs == 5) {

            // get parameter 3 "position" as integer
            SQInteger position;
            if (SQ_FAILED(sq_getinteger(vm, 4, &position))){
                return sq_throwerror(vm, "argument 3 \"position\" is not of type integer");
            }

            // get parameter 4 "division" as integer
            SQInteger division;
            if (SQ_FAILED(sq_getinteger(vm, 5, &division))){
                return sq_throwerror(vm, "argument 4 \"division\" is not of type integer");
            }

            // call the implementation
            try {
                obj->schedule(*note, bar, position, division);
            }
            catch(std::exception const& e) {
                return sq_throwerror(vm, e.what());
            }
        }

        // 5 parameters passed in
        else if(numargs == 6) {

            // get parameter 3 "position" as integer
            SQInteger position;
            if (SQ_FAILED(sq_getinteger(vm, 4, &position))){
                return sq_throwerror(vm, "argument 3 \"position\" is not of type integer");
            }

            // get parameter 4 "division" as integer
            SQInteger division;
            if (SQ_FAILED(sq_getinteger(vm, 5, &division))){
                return sq_throwerror(vm, "argument 4 \"division\" is not of type integer");
            }

            // get parameter 5 "channel" as integer
            SQInteger channel;
            if (SQ_FAILED(sq_getinteger(vm, 6, &channel))){
                return sq_throwerror(vm, "argument 5 \"channel\" is not of type integer");
            }

            // call the implementation
            try {
                obj->schedule(*note, bar, position, division, channel);
            }
            catch(std::exception const& e) {
                return sq_throwerror(vm, e.what());
            }
        }

        else {
            // call the implementation
            try {
                obj->schedule(*note, bar);
            }
            catch(std::exception const& e) {
                return sq_throwerror(vm, e.what());
            }
        }

        // void method, returns no value
        return 0;
    }
    else if(overrideType == OT_INSTANCE && overrideTypeTag == &MidiPatternObject) {
        SQInteger numargs = sq_gettop(vm);
        // check parameter count
        if(numargs > 6) {
            return sq_throwerror(vm, "too many parameters, expected at most 5");
        }
        if(numargs < 3) {
            return sq_throwerror(vm, "insufficient parameters, expected at least 2");
        }
        // get "this" pointer
        SQUserPointer userPtr = 0;
        if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
            return sq_throwerror(vm, "schedule method needs an instance of SystemOut");
        }
        MidiOutputPort *obj = static_cast<MidiOutputPort*>(userPtr);

        // get parameter 1 "pattern" as Midi.Pattern
        Pattern *pattern = 0;
        sq_getinstanceup(vm, 2, (SQUserPointer*)&pattern, 0);
        if(pattern == 0) {
            return sq_throwerror(vm, "argument 1 \"pattern\" is not of type Midi.Pattern");
        }

        // get parameter 2 "bar" as integer
        SQInteger bar;
        if (SQ_FAILED(sq_getinteger(vm, 3, &bar))){
            return sq_throwerror(vm, "argument 2 \"bar\" is not of type integer");
        }

        // 3 parameters passed in
        if(numargs == 4) {

            // get parameter 3 "position" as integer
            SQInteger position;
            if (SQ_FAILED(sq_getinteger(vm, 4, &position))){
                return sq_throwerror(vm, "argument 3 \"position\" is not of type integer");
            }

            // call the implementation
            try {
                obj->schedule(*pattern, bar, position);
            }
            catch(std::exception const& e) {
                return sq_throwerror(vm, e.what());
            }
        }

        // 4 parameters passed in
        else if(numargs == 5) {

            // get parameter 3 "position" as integer
            SQInteger position;
            if (SQ_FAILED(sq_getinteger(vm, 4, &position))){
                return sq_throwerror(vm, "argument 3 \"position\" is not of type integer");
            }

            // get parameter 4 "division" as integer
            SQInteger division;
            if (SQ_FAILED(sq_getinteger(vm, 5, &division))){
                return sq_throwerror(vm, "argument 4 \"division\" is not of type integer");
            }

            // call the implementation
            try {
                obj->schedule(*pattern, bar, position, division);
            }
            catch(std::exception const& e) {
                return sq_throwerror(vm, e.what());
            }
        }

        // 5 parameters passed in
        else if(numargs == 6) {

            // get parameter 3 "position" as integer
            SQInteger position;
            if (SQ_FAILED(sq_getinteger(vm, 4, &position))){
                return sq_throwerror(vm, "argument 3 \"position\" is not of type integer");
            }

            // get parameter 4 "division" as integer
            SQInteger division;
            if (SQ_FAILED(sq_getinteger(vm, 5, &division))){
                return sq_throwerror(vm, "argument 4 \"division\" is not of type integer");
            }

            // get parameter 5 "channel" as integer
            SQInteger channel;
            if (SQ_FAILED(sq_getinteger(vm, 6, &channel))){
                return sq_throwerror(vm, "argument 5 \"channel\" is not of type integer");
            }

            // call the implementation
            try {
                obj->schedule(*pattern, bar, position, division, channel);
            }
            catch(std::exception const& e) {
                return sq_throwerror(vm, e.what());
            }
        }

        else {
            // call the implementation
            try {
                obj->schedule(*pattern, bar);
            }
            catch(std::exception const& e) {
                return sq_throwerror(vm, e.what());
            }
        }

        // void method, returns no value
        return 0;
    }
    else if(MidiMessage *message = getMidiMessage(vm)) {
        SQInteger numargs = sq_gettop(vm);
        // check parameter count
        if(numargs > 6) {
            return sq_throwerror(vm, "too many parameters, expected at most 5");
        }
        if(numargs < 3) {
            return sq_throwerror(vm, "insufficient parameters, expected at least 2");
        }
        // get "this" pointer
        SQUserPointer userPtr = 0;
        if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
            return sq_throwerror(vm, "schedule method needs an instance of SystemOut");
        }
        MidiOutputPort *obj = static_cast<MidiOutputPort*>(userPtr);

        // get parameter 2 "bar" as integer
        SQInteger bar;
        if (SQ_FAILED(sq_getinteger(vm, 3, &bar))){
            return sq_throwerror(vm, "argument 2 \"bar\" is not of type integer");
        }

        // 3 parameters passed in
        if(numargs == 4) {

            // get parameter 3 "position" as integer
            SQInteger position;
            if (SQ_FAILED(sq_getinteger(vm, 4, &position))){
                return sq_throwerror(vm, "argument 3 \"position\" is not of type integer");
            }

            // call the implementation
            try {
                obj->schedule(*message, bar, position);
            }
            catch(std::exception const& e) {
                return sq_throwerror(vm, e.what());
            }
        }

        // 4 parameters passed in
        else if(numargs == 5) {

            // get parameter 3 "position" as integer
            SQInteger position;
            if (SQ_FAILED(sq_getinteger(vm, 4, &position))){
                return sq_throwerror(vm, "argument 3 \"position\" is not of type integer");
            }

            // get parameter 4 "division" as integer
            SQInteger division;
            if (SQ_FAILED(sq_getinteger(vm, 5, &division))){
                return sq_throwerror(vm, "argument 4 \"division\" is not of type integer");
            }

            // call the implementation
            try {
                obj->schedule(*message, bar, position, division);
            }
            catch(std::exception const& e) {
                return sq_throwerror(vm, e.what());
            }
        }

        // 5 parameters passed in
        else if(numargs == 6) {

            // get parameter 3 "position" as integer
            SQInteger position;
            if (SQ_FAILED(sq_getinteger(vm, 4, &position))){
                return sq_throwerror(vm, "argument 3 \"position\" is not of type integer");
            }

            // get parameter 4 "division" as integer
            SQInteger division;
            if (SQ_FAILED(sq_getinteger(vm, 5, &division))){
                return sq_throwerror(vm, "argument 4 \"division\" is not of type integer");
            }

            // get parameter 5 "channel" as integer
            SQInteger channel;
            if (SQ_FAILED(sq_getinteger(vm, 6, &channel))){
                return sq_throwerror(vm, "argument 5 \"channel\" is not of type integer");
            }

            // call the implementation
            try {
                obj->schedule(*message, bar, position, division, channel);
            }
            catch(std::exception const& e) {
                return sq_throwerror(vm, e.what());
            }
        }

        else {
            // call the implementation
            try {
                obj->schedule(*message, bar);
            }
            catch(std::exception const& e) {
                return sq_throwerror(vm, e.what());
            }
        }

        // void method, returns no value
        return 0;
    }
    else {
        return sq_throwerror(vm, "argument 1 is not of type {Midi.Note, Midi.Pattern, Midi.Message}");
    }
}

//
// Midi.PitchBend class
//
SQInteger MidiPitchBendRelease(SQUserPointer p, SQInteger size)
{
    delete static_cast<PitchBend*>(p);
}

SQInteger MidiPitchBendCtor(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 2) {
        return sq_throwerror(vm, "too many parameters, expected at most 1");
    }
    if(numargs < 2) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 1");
    }
    // get parameter 1 "program" as integer
    SQInteger program;
    if (SQ_FAILED(sq_getinteger(vm, 2, &program))){
        return sq_throwerror(vm, "argument 1 \"program\" is not of type integer");
    }

    PitchBend *obj;
    // call the implementation
    try {
        obj = new PitchBend(program);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // return pointer to new object
    sq_setinstanceup(vm, 1, (SQUserPointer*)obj);
    sq_setreleasehook(vm, 1, MidiPitchBendRelease);
    return 1;
}

//
// Midi.ProgramChange class
//
SQInteger MidiProgramChangeRelease(SQUserPointer p, SQInteger size)
{
    delete static_cast<ProgramChange*>(p);
}

SQInteger MidiProgramChangeCtor(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 2) {
        return sq_throwerror(vm, "too many parameters, expected at most 1");
    }
    if(numargs < 2) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 1");
    }
    // get parameter 1 "program" as integer
    SQInteger program;
    if (SQ_FAILED(sq_getinteger(vm, 2, &program))){
        return sq_throwerror(vm, "argument 1 \"program\" is not of type integer");
    }

    ProgramChange *obj;
    // call the implementation
    try {
        obj = new ProgramChange(program);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // return pointer to new object
    sq_setinstanceup(vm, 1, (SQUserPointer*)obj);
    sq_setreleasehook(vm, 1, MidiProgramChangeRelease);
    return 1;
}

//
// Midi.BeatTracker class
//
SQInteger MidiBeatTrackerCtor(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 4) {
        return sq_throwerror(vm, "too many parameters, expected at most 3");
    }
    if(numargs < 2) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 1");
    }
    // get parameter 1 "bpm" as float
    SQFloat bpm;
    if (SQ_FAILED(sq_getfloat(vm, 2, &bpm))){
        return sq_throwerror(vm, "argument 1 \"bpm\" is not of type float");
    }

    MidiBeatTracker *obj;
    // 2 parameters passed in
    if(numargs == 3) {

        // get parameter 2 "numerator" as integer
        SQInteger numerator;
        if (SQ_FAILED(sq_getinteger(vm, 3, &numerator))){
            return sq_throwerror(vm, "argument 2 \"numerator\" is not of type integer");
        }

        // call the implementation
        try {
            obj = MidiBeatTrackerCache::instance().getMidiBeatTracker(bpm, numerator);
        }
        catch(std::exception const& e) {
            return sq_throwerror(vm, e.what());
        }
    }

    // 3 parameters passed in
    else if(numargs == 4) {

        // get parameter 2 "numerator" as integer
        SQInteger numerator;
        if (SQ_FAILED(sq_getinteger(vm, 3, &numerator))){
            return sq_throwerror(vm, "argument 2 \"numerator\" is not of type integer");
        }

        // get parameter 3 "denominator" as integer
        SQInteger denominator;
        if (SQ_FAILED(sq_getinteger(vm, 4, &denominator))){
            return sq_throwerror(vm, "argument 3 \"denominator\" is not of type integer");
        }

        // call the implementation
        try {
            obj = MidiBeatTrackerCache::instance().getMidiBeatTracker(bpm, numerator, denominator);
        }
        catch(std::exception const& e) {
            return sq_throwerror(vm, e.what());
        }
    }

    else {
        // call the implementation
        try {
            obj = MidiBeatTrackerCache::instance().getMidiBeatTracker(bpm);
        }
        catch(std::exception const& e) {
            return sq_throwerror(vm, e.what());
        }
    }

    // return pointer to new object
    sq_setinstanceup(vm, 1, (SQUserPointer*)obj);
    return 1;
}

//
// Midi.BeatTracker connectMidi
//
SQInteger MidiBeatTrackerconnectMidi(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 2) {
        return sq_throwerror(vm, "too many parameters, expected at most 1");
    }
    if(numargs < 2) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 1");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "connectMidi method needs an instance of BeatTracker");
    }
    MidiBeatTracker *obj = static_cast<MidiBeatTracker*>(userPtr);

    // get parameter 1 "source" as Midi.Source
    MidiSource *source = getMidiSource(vm);
    if(source == 0) {
        return sq_throwerror(vm, "argument 1 \"source\" is not of type Midi.Source");
    }

    // call the implementation
    try {
        obj->connectMidi(*source);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // void method, returns no value
    return 0;
}

//
// Midi.BeatTracker countIn
//
SQInteger MidiBeatTrackercountIn(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 2) {
        return sq_throwerror(vm, "too many parameters, expected at most 1");
    }
    if(numargs < 2) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 1");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "countIn method needs an instance of BeatTracker");
    }
    MidiBeatTracker *obj = static_cast<MidiBeatTracker*>(userPtr);

    // get parameter 1 "note" as integer
    SQInteger note;
    if (SQ_FAILED(sq_getinteger(vm, 2, &note))){
        return sq_throwerror(vm, "argument 1 \"note\" is not of type integer");
    }

    // call the implementation
    try {
        obj->countIn(note);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // void method, returns no value
    return 0;
}

//
// Midi.BeatTracker noteWeight
//
SQInteger MidiBeatTrackernoteWeight(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 3) {
        return sq_throwerror(vm, "too many parameters, expected at most 2");
    }
    if(numargs < 3) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 2");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "noteWeight method needs an instance of BeatTracker");
    }
    MidiBeatTracker *obj = static_cast<MidiBeatTracker*>(userPtr);

    // get parameter 1 "note" as integer
    SQInteger note;
    if (SQ_FAILED(sq_getinteger(vm, 2, &note))){
        return sq_throwerror(vm, "argument 1 \"note\" is not of type integer");
    }

    // get parameter 2 "weight" as float
    SQFloat weight;
    if (SQ_FAILED(sq_getfloat(vm, 3, &weight))){
        return sq_throwerror(vm, "argument 2 \"weight\" is not of type float");
    }

    // call the implementation
    try {
        obj->setNoteWeight(note, weight);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // void method, returns no value
    return 0;
}

//
// Midi.BeatTracker onBeat
//
SQInteger MidiBeatTrackeronBeat(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 2) {
        return sq_throwerror(vm, "too many parameters, expected at most 1");
    }
    if(numargs < 2) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 1");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "onBeat method needs an instance of BeatTracker");
    }
    MidiBeatTracker *obj = static_cast<MidiBeatTracker*>(userPtr);

    // get parameter 1 "handler" as function
    HSQOBJECT handlerObj;
    if (SQ_FAILED(sq_getstackobj(vm, 2, &handlerObj))) {
        return sq_throwerror(vm, "argument 1 \"handler\" is not of type function");
    }
    if (sq_gettype(vm, 2) != OT_CLOSURE) {
        return sq_throwerror(vm, "argument 1 \"handler\" is not of type function");
    }
    SQUnsignedInteger nparams, nfreevars;
    sq_getclosureinfo(vm, 2, &nparams, &nfreevars);
    sq_addref(vm, &handlerObj);
    ScriptFunction handler(vm, handlerObj, nparams);

    // call the implementation
    try {
        obj->onBeat(handler);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // void method, returns no value
    return 0;
}

//
// Midi.BeatTracker onCount
//
SQInteger MidiBeatTrackeronCount(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 2) {
        return sq_throwerror(vm, "too many parameters, expected at most 1");
    }
    if(numargs < 2) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 1");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "onCount method needs an instance of BeatTracker");
    }
    MidiBeatTracker *obj = static_cast<MidiBeatTracker*>(userPtr);

    // get parameter 1 "handler" as function
    HSQOBJECT handlerObj;
    if (SQ_FAILED(sq_getstackobj(vm, 2, &handlerObj))) {
        return sq_throwerror(vm, "argument 1 \"handler\" is not of type function");
    }
    if (sq_gettype(vm, 2) != OT_CLOSURE) {
        return sq_throwerror(vm, "argument 1 \"handler\" is not of type function");
    }
    SQUnsignedInteger nparams, nfreevars;
    sq_getclosureinfo(vm, 2, &nparams, &nfreevars);
    sq_addref(vm, &handlerObj);
    ScriptFunction handler(vm, handlerObj, nparams);

    // call the implementation
    try {
        obj->onCount(handler);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // void method, returns no value
    return 0;
}

//
// Midi.BeatTracker stopOnSilence
//
SQInteger MidiBeatTrackerstopOnSilence(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 2) {
        return sq_throwerror(vm, "too many parameters, expected at most 1");
    }
    if(numargs < 2) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 1");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "stopOnSilence method needs an instance of BeatTracker");
    }
    MidiBeatTracker *obj = static_cast<MidiBeatTracker*>(userPtr);

    // get parameter 1 "seconds" as integer
    SQInteger seconds;
    if (SQ_FAILED(sq_getinteger(vm, 2, &seconds))){
        return sq_throwerror(vm, "argument 1 \"seconds\" is not of type integer");
    }

    // call the implementation
    try {
        obj->stopOnSilence(seconds);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // void method, returns no value
    return 0;
}


void bindMidi(HSQUIRRELVM vm)
{
    // create package table
    sq_pushstring(vm, "Midi", -1);
    sq_newtable(vm);

    // create class Midi.ABCReader
    sq_pushstring(vm, "ABCReader", -1);
    sq_newclass(vm, false);
    sq_getstackobj(vm, -1, &MidiABCReaderObject);
    sq_settypetag(vm, -1, &MidiABCReaderObject);

    // ctor for class ABCReader
    sq_pushstring(vm, _SC("constructor"), -1);
    sq_newclosure(vm, &MidiABCReaderCtor, 0);
    sq_newslot(vm, -3, false);

    // methods for class ABCReader
    sq_pushstring(vm, _SC("read"), -1);
    sq_newclosure(vm, &MidiABCReaderread, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("readTune"), -1);
    sq_newclosure(vm, &MidiABCReaderreadTune, 0);
    sq_newslot(vm, -3, false);

    // push ABCReader to Midi package table
    sq_newslot(vm, -3, false);

    // create class Midi.ChordReader
    sq_pushstring(vm, "ChordReader", -1);
    sq_newclass(vm, false);
    sq_getstackobj(vm, -1, &MidiChordReaderObject);
    sq_settypetag(vm, -1, &MidiChordReaderObject);

    // ctor for class ChordReader
    sq_pushstring(vm, _SC("constructor"), -1);
    sq_newclosure(vm, &MidiChordReaderCtor, 0);
    sq_newslot(vm, -3, false);

    // methods for class ChordReader
    sq_pushstring(vm, _SC("read"), -1);
    sq_newclosure(vm, &MidiChordReaderread, 0);
    sq_newslot(vm, -3, false);

    // push ChordReader to Midi package table
    sq_newslot(vm, -3, false);

    // create class Midi.DrumTabReader
    sq_pushstring(vm, "DrumTabReader", -1);
    sq_newclass(vm, false);
    sq_getstackobj(vm, -1, &MidiDrumTabReaderObject);
    sq_settypetag(vm, -1, &MidiDrumTabReaderObject);

    // ctor for class DrumTabReader
    sq_pushstring(vm, _SC("constructor"), -1);
    sq_newclosure(vm, &MidiDrumTabReaderCtor, 0);
    sq_newslot(vm, -3, false);

    // methods for class DrumTabReader
    sq_pushstring(vm, _SC("read"), -1);
    sq_newclosure(vm, &MidiDrumTabReaderread, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("velocity"), -1);
    sq_newclosure(vm, &MidiDrumTabReadervelocity, 0);
    sq_newslot(vm, -3, false);

    // push DrumTabReader to Midi package table
    sq_newslot(vm, -3, false);

    // create class Midi.SystemIn
    sq_pushstring(vm, "SystemIn", -1);
    sq_newclass(vm, false);
    sq_getstackobj(vm, -1, &MidiSystemInObject);
    sq_settypetag(vm, -1, &MidiSystemInObject);

    // ctor for class SystemIn
    sq_pushstring(vm, _SC("constructor"), -1);
    sq_newclosure(vm, &MidiSystemInCtor, 0);
    sq_newslot(vm, -3, false);

    // methods for class SystemIn
    sq_pushstring(vm, _SC("midiOutput"), -1);
    sq_newclosure(vm, &MidiSystemInmidiOutput, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("onControl"), -1);
    sq_newclosure(vm, &MidiSystemInonControl, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("onNoteOff"), -1);
    sq_newclosure(vm, &MidiSystemInonNoteOff, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("onNoteOn"), -1);
    sq_newclosure(vm, &MidiSystemInonNoteOn, 0);
    sq_newslot(vm, -3, false);

    // push SystemIn to Midi package table
    sq_newslot(vm, -3, false);

    // create class Midi.Note
    sq_pushstring(vm, "Note", -1);
    sq_newclass(vm, false);
    sq_getstackobj(vm, -1, &MidiNoteObject);
    sq_settypetag(vm, -1, &MidiNoteObject);

    // ctor for class Note
    sq_pushstring(vm, _SC("constructor"), -1);
    sq_newclosure(vm, &MidiNoteCtor, 0);
    sq_newslot(vm, -3, false);

    // methods for class Note
    sq_pushstring(vm, _SC("pitch"), -1);
    sq_newclosure(vm, &MidiNotepitch, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("transpose"), -1);
    sq_newclosure(vm, &MidiNotetranspose, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("velocity"), -1);
    sq_newclosure(vm, &MidiNotevelocity, 0);
    sq_newslot(vm, -3, false);

    // push Note to Midi package table
    sq_newslot(vm, -3, false);

    // create class Midi.NoteOn
    sq_pushstring(vm, "NoteOn", -1);
    sq_newclass(vm, false);
    sq_getstackobj(vm, -1, &MidiNoteOnObject);
    sq_settypetag(vm, -1, &MidiNoteOnObject);

    // ctor for class NoteOn
    sq_pushstring(vm, _SC("constructor"), -1);
    sq_newclosure(vm, &MidiNoteOnCtor, 0);
    sq_newslot(vm, -3, false);

    // methods for class NoteOn
    sq_pushstring(vm, _SC("pitch"), -1);
    sq_newclosure(vm, &MidiNoteOnpitch, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("velocity"), -1);
    sq_newclosure(vm, &MidiNoteOnvelocity, 0);
    sq_newslot(vm, -3, false);

    // push NoteOn to Midi package table
    sq_newslot(vm, -3, false);

    // create class Midi.NoteOff
    sq_pushstring(vm, "NoteOff", -1);
    sq_newclass(vm, false);
    sq_getstackobj(vm, -1, &MidiNoteOffObject);
    sq_settypetag(vm, -1, &MidiNoteOffObject);

    // ctor for class NoteOff
    sq_pushstring(vm, _SC("constructor"), -1);
    sq_newclosure(vm, &MidiNoteOffCtor, 0);
    sq_newslot(vm, -3, false);

    // methods for class NoteOff
    sq_pushstring(vm, _SC("pitch"), -1);
    sq_newclosure(vm, &MidiNoteOffpitch, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("velocity"), -1);
    sq_newclosure(vm, &MidiNoteOffvelocity, 0);
    sq_newslot(vm, -3, false);

    // push NoteOff to Midi package table
    sq_newslot(vm, -3, false);

    // create class Midi.Control
    sq_pushstring(vm, "Control", -1);
    sq_newclass(vm, false);
    sq_getstackobj(vm, -1, &MidiControlObject);
    sq_settypetag(vm, -1, &MidiControlObject);

    // ctor for class Control
    sq_pushstring(vm, _SC("constructor"), -1);
    sq_newclosure(vm, &MidiControlCtor, 0);
    sq_newslot(vm, -3, false);

    // methods for class Control
    sq_pushstring(vm, _SC("controller"), -1);
    sq_newclosure(vm, &MidiControlcontroller, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("value"), -1);
    sq_newclosure(vm, &MidiControlvalue, 0);
    sq_newslot(vm, -3, false);

    // push Control to Midi package table
    sq_newslot(vm, -3, false);

    // create class Midi.MMLReader
    sq_pushstring(vm, "MMLReader", -1);
    sq_newclass(vm, false);
    sq_getstackobj(vm, -1, &MidiMMLReaderObject);
    sq_settypetag(vm, -1, &MidiMMLReaderObject);

    // ctor for class MMLReader
    sq_pushstring(vm, _SC("constructor"), -1);
    sq_newclosure(vm, &MidiMMLReaderCtor, 0);
    sq_newslot(vm, -3, false);

    // methods for class MMLReader
    sq_pushstring(vm, _SC("read"), -1);
    sq_newclosure(vm, &MidiMMLReaderread, 0);
    sq_newslot(vm, -3, false);

    // push MMLReader to Midi package table
    sq_newslot(vm, -3, false);

    // create class Midi.Output
    sq_pushstring(vm, "Output", -1);
    sq_newclass(vm, false);
    sq_getstackobj(vm, -1, &MidiOutputObject);
    sq_settypetag(vm, -1, &MidiOutputObject);

    // ctor for class Output
    sq_pushstring(vm, _SC("constructor"), -1);
    sq_newclosure(vm, &MidiOutputCtor, 0);
    sq_newslot(vm, -3, false);

    // methods for class Output
    sq_pushstring(vm, _SC("onControl"), -1);
    sq_newclosure(vm, &MidiOutputonControl, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("onNoteOff"), -1);
    sq_newclosure(vm, &MidiOutputonNoteOff, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("onNoteOn"), -1);
    sq_newclosure(vm, &MidiOutputonNoteOn, 0);
    sq_newslot(vm, -3, false);

    // push Output to Midi package table
    sq_newslot(vm, -3, false);

    // create class Midi.Pattern
    sq_pushstring(vm, "Pattern", -1);
    sq_newclass(vm, false);
    sq_getstackobj(vm, -1, &MidiPatternObject);
    sq_settypetag(vm, -1, &MidiPatternObject);

    // ctor for class Pattern
    sq_pushstring(vm, _SC("constructor"), -1);
    sq_newclosure(vm, &MidiPatternCtor, 0);
    sq_newslot(vm, -3, false);

    // methods for class Pattern
    sq_pushstring(vm, _SC("add"), -1);
    sq_newclosure(vm, &MidiPatternadd, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("note"), -1);
    sq_newclosure(vm, &MidiPatternnote, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("print"), -1);
    sq_newclosure(vm, &MidiPatternprint, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("size"), -1);
    sq_newclosure(vm, &MidiPatternsize, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("transpose"), -1);
    sq_newclosure(vm, &MidiPatterntranspose, 0);
    sq_newslot(vm, -3, false);

    // push Pattern to Midi package table
    sq_newslot(vm, -3, false);

    // create class Midi.Tune
    sq_pushstring(vm, "Tune", -1);
    sq_newclass(vm, false);
    sq_getstackobj(vm, -1, &MidiTuneObject);
    sq_settypetag(vm, -1, &MidiTuneObject);

    // ctor for class Tune
    sq_pushstring(vm, _SC("constructor"), -1);
    sq_newclosure(vm, &MidiTuneCtor, 0);
    sq_newslot(vm, -3, false);

    // methods for class Tune
    sq_pushstring(vm, _SC("timeSignature"), -1);
    sq_newclosure(vm, &MidiTunetimeSignature, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("title"), -1);
    sq_newclosure(vm, &MidiTunetitle, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("track"), -1);
    sq_newclosure(vm, &MidiTunetrack, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("trackCount"), -1);
    sq_newclosure(vm, &MidiTunetrackCount, 0);
    sq_newslot(vm, -3, false);

    // push Tune to Midi package table
    sq_newslot(vm, -3, false);

    // create class Midi.SystemOut
    sq_pushstring(vm, "SystemOut", -1);
    sq_newclass(vm, false);
    sq_getstackobj(vm, -1, &MidiSystemOutObject);
    sq_settypetag(vm, -1, &MidiSystemOutObject);

    // ctor for class SystemOut
    sq_pushstring(vm, _SC("constructor"), -1);
    sq_newclosure(vm, &MidiSystemOutCtor, 0);
    sq_newslot(vm, -3, false);

    // methods for class SystemOut
    sq_pushstring(vm, _SC("midiChannel"), -1);
    sq_newclosure(vm, &MidiSystemOutmidiChannel, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("schedule"), -1);
    sq_newclosure(vm, &MidiSystemOutschedule, 0);
    sq_newslot(vm, -3, false);

    // push SystemOut to Midi package table
    sq_newslot(vm, -3, false);

    // create class Midi.PitchBend
    sq_pushstring(vm, "PitchBend", -1);
    sq_newclass(vm, false);
    sq_getstackobj(vm, -1, &MidiPitchBendObject);
    sq_settypetag(vm, -1, &MidiPitchBendObject);

    // ctor for class PitchBend
    sq_pushstring(vm, _SC("constructor"), -1);
    sq_newclosure(vm, &MidiPitchBendCtor, 0);
    sq_newslot(vm, -3, false);

    // methods for class PitchBend
    // push PitchBend to Midi package table
    sq_newslot(vm, -3, false);

    // create class Midi.ProgramChange
    sq_pushstring(vm, "ProgramChange", -1);
    sq_newclass(vm, false);
    sq_getstackobj(vm, -1, &MidiProgramChangeObject);
    sq_settypetag(vm, -1, &MidiProgramChangeObject);

    // ctor for class ProgramChange
    sq_pushstring(vm, _SC("constructor"), -1);
    sq_newclosure(vm, &MidiProgramChangeCtor, 0);
    sq_newslot(vm, -3, false);

    // methods for class ProgramChange
    // push ProgramChange to Midi package table
    sq_newslot(vm, -3, false);

    // create class Midi.BeatTracker
    sq_pushstring(vm, "BeatTracker", -1);
    sq_newclass(vm, false);
    sq_getstackobj(vm, -1, &MidiBeatTrackerObject);
    sq_settypetag(vm, -1, &MidiBeatTrackerObject);

    // ctor for class BeatTracker
    sq_pushstring(vm, _SC("constructor"), -1);
    sq_newclosure(vm, &MidiBeatTrackerCtor, 0);
    sq_newslot(vm, -3, false);

    // methods for class BeatTracker
    sq_pushstring(vm, _SC("connectMidi"), -1);
    sq_newclosure(vm, &MidiBeatTrackerconnectMidi, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("countIn"), -1);
    sq_newclosure(vm, &MidiBeatTrackercountIn, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("noteWeight"), -1);
    sq_newclosure(vm, &MidiBeatTrackernoteWeight, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("onBeat"), -1);
    sq_newclosure(vm, &MidiBeatTrackeronBeat, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("onCount"), -1);
    sq_newclosure(vm, &MidiBeatTrackeronCount, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("stopOnSilence"), -1);
    sq_newclosure(vm, &MidiBeatTrackerstopOnSilence, 0);
    sq_newslot(vm, -3, false);

    // push BeatTracker to Midi package table
    sq_newslot(vm, -3, false);

    // push package "Midi" to root table
    sq_newslot(vm, -3, false);
}
}