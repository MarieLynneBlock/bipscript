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
#include "bindings.h"

#include "abcreader.h"
#include "midiport.h"
#include "module_midi.h"
#include "midiinputbuffer.h"
#include "miditune.h"
#include "drumtabreader.h"

namespace binding {

// object references to types in this package
HSQOBJECT MidiABCReaderObject;
HSQOBJECT MidiInputObject;
HSQOBJECT MidiNoteObject;
HSQOBJECT MidiControlObject;
HSQOBJECT MidiInputBufferObject;
HSQOBJECT MidiPatternObject;
HSQOBJECT MidiTuneObject;
HSQOBJECT MidiOutputObject;
HSQOBJECT MidiProgramChangeObject;
HSQOBJECT MidiDrumTabReaderObject;

//
// Midi.ABCReader class
//
SQInteger MidiABCReaderCtor(HSQUIRRELVM vm)
{
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
    //sq_setreleasehook(vm, 1, release_hook);
    return 1;
}

//
// Midi.ABCReader read
//
SQInteger MidiABCReaderread(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs < 2) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 1");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    sq_getinstanceup(vm, 1, &userPtr, 0);
    ABCReader *obj = static_cast<ABCReader*>(userPtr);

    // get parameter 1 "abc" as string
    const SQChar* abc;
    if (SQ_FAILED(sq_getstring(vm, 2, &abc))){
        return sq_throwerror(vm, "argument 1 is not of type string");
    }

    // return value
    Pattern* ret;
    // 2 parameters passed in
    if(numargs == 3) {

        // get parameter 2 "key" as string
        const SQChar* key;
        if (SQ_FAILED(sq_getstring(vm, 3, &key))){
            return sq_throwerror(vm, "argument 2 is not of type string");
        }

        // call the implementation
        try {
            ret = obj->read(abc, key);
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
    //sq_setreleasehook(vm, -1, &?);

    return 1;
}

//
// Midi.ABCReader readTune
//
SQInteger MidiABCReaderreadTune(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs < 2) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 1");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    sq_getinstanceup(vm, 1, &userPtr, 0);
    ABCReader *obj = static_cast<ABCReader*>(userPtr);

    // get parameter 1 "abc" as string
    const SQChar* abc;
    if (SQ_FAILED(sq_getstring(vm, 2, &abc))){
        return sq_throwerror(vm, "argument 1 is not of type string");
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
    //sq_setreleasehook(vm, -1, &?);

    return 1;
}

//
// Midi.Input class
//
SQInteger MidiInputCtor(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs < 2) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 1");
    }
    // get parameter 1 "name" as string
    const SQChar* name;
    if (SQ_FAILED(sq_getstring(vm, 2, &name))){
        return sq_throwerror(vm, "argument 1 is not of type string");
    }

    MidiInputPort *obj;
    // 2 parameters passed in
    if(numargs == 3) {

        // get parameter 2 "connection" as string
        const SQChar* connection;
        if (SQ_FAILED(sq_getstring(vm, 3, &connection))){
            return sq_throwerror(vm, "argument 2 is not of type string");
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
    //sq_setreleasehook(vm, 1, release_hook);
    return 1;
}

//
// Midi.Note class
//
SQInteger MidiNoteCtor(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs < 5) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 4");
    }
    // get parameter 1 "pitch" as integer
    SQInteger pitch;
    if (SQ_FAILED(sq_getinteger(vm, 2, &pitch))){
        return sq_throwerror(vm, "argument 1 is not of type integer");
    }

    // get parameter 2 "velocity" as integer
    SQInteger velocity;
    if (SQ_FAILED(sq_getinteger(vm, 3, &velocity))){
        return sq_throwerror(vm, "argument 2 is not of type integer");
    }

    // get parameter 3 "duration" as integer
    SQInteger duration;
    if (SQ_FAILED(sq_getinteger(vm, 4, &duration))){
        return sq_throwerror(vm, "argument 3 is not of type integer");
    }

    // get parameter 4 "division" as integer
    SQInteger division;
    if (SQ_FAILED(sq_getinteger(vm, 5, &division))){
        return sq_throwerror(vm, "argument 4 is not of type integer");
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
    //sq_setreleasehook(vm, 1, release_hook);
    return 1;
}

//
// Midi.Control class
//
SQInteger MidiControlCtor(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs < 3) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 2");
    }
    // get parameter 1 "control" as integer
    SQInteger control;
    if (SQ_FAILED(sq_getinteger(vm, 2, &control))){
        return sq_throwerror(vm, "argument 1 is not of type integer");
    }

    // get parameter 2 "value" as integer
    SQInteger value;
    if (SQ_FAILED(sq_getinteger(vm, 3, &value))){
        return sq_throwerror(vm, "argument 2 is not of type integer");
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
    //sq_setreleasehook(vm, 1, release_hook);
    return 1;
}

//
// Midi.InputBuffer class
//
SQInteger MidiInputBufferCtor(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs < 2) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 1");
    }
    // get parameter 1 "source" as EventSource
    SQUserPointer sourceTypeTag, sourcePtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 2, (SQUserPointer*)&sourcePtr, 0))) {
        return sq_throwerror(vm, "argument 1 is not an object of type EventSource");
    }
    sq_gettypetag(vm, 2, &sourceTypeTag);
    EventSource *source = getEventSource(sourcePtr, sourceTypeTag);
    if(source == 0) {
        return sq_throwerror(vm, "argument 1 is not of type EventSource");
    }

    MidiInputBuffer *obj;
    // call the implementation
    try {
        obj = MidiInputBufferCache::instance().getMidiInputBuffer(*source);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // return pointer to new object
    sq_setinstanceup(vm, 1, (SQUserPointer*)obj);
    //sq_setreleasehook(vm, 1, release_hook);
    return 1;
}

//
// Midi.InputBuffer lastControlValue
//
SQInteger MidiInputBufferlastControlValue(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs < 2) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 1");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    sq_getinstanceup(vm, 1, &userPtr, 0);
    MidiInputBuffer *obj = static_cast<MidiInputBuffer*>(userPtr);

    // get parameter 1 "control" as integer
    SQInteger control;
    if (SQ_FAILED(sq_getinteger(vm, 2, &control))){
        return sq_throwerror(vm, "argument 1 is not of type integer");
    }

    // return value
    SQInteger ret;
    // call the implementation
    try {
        ret = obj->lastControlValue(control);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // push return value
    sq_pushinteger(vm, ret);
    return 1;
}

//
// Midi.Pattern class
//
SQInteger MidiPatternCtor(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    Pattern *obj;
    // 1 parameters passed in
    if(numargs == 2) {

        // get parameter 1 "other" as Midi.Pattern
        Pattern *other = 0;
        sq_getinstanceup(vm, 2, (SQUserPointer*)&other, 0);
        if(other == 0) {
            return sq_throwerror(vm, "argument 1 is not of type Midi.Pattern");
        }
        SQUserPointer otherTypeTag;
        sq_gettypetag(vm, 2, &otherTypeTag);
        if(otherTypeTag != &MidiPatternObject) {
            return sq_throwerror(vm, "argument 1 is not of type Midi.Pattern");
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
    //sq_setreleasehook(vm, 1, release_hook);
    return 1;
}

//
// Midi.Pattern add
//
SQInteger MidiPatternadd(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs < 5) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 4");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    sq_getinstanceup(vm, 1, &userPtr, 0);
    Pattern *obj = static_cast<Pattern*>(userPtr);

    // get parameter 1 "note" as Midi.Note
    Note *note = 0;
    sq_getinstanceup(vm, 2, (SQUserPointer*)&note, 0);
    if(note == 0) {
        return sq_throwerror(vm, "argument 1 is not of type Midi.Note");
    }
    SQUserPointer noteTypeTag;
    sq_gettypetag(vm, 2, &noteTypeTag);
    if(noteTypeTag != &MidiNoteObject) {
        return sq_throwerror(vm, "argument 1 is not of type Midi.Note");
    }

    // get parameter 2 "bar" as integer
    SQInteger bar;
    if (SQ_FAILED(sq_getinteger(vm, 3, &bar))){
        return sq_throwerror(vm, "argument 2 is not of type integer");
    }

    // get parameter 3 "position" as integer
    SQInteger position;
    if (SQ_FAILED(sq_getinteger(vm, 4, &position))){
        return sq_throwerror(vm, "argument 3 is not of type integer");
    }

    // get parameter 4 "division" as integer
    SQInteger division;
    if (SQ_FAILED(sq_getinteger(vm, 5, &division))){
        return sq_throwerror(vm, "argument 4 is not of type integer");
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
// Midi.Pattern getSize
//
SQInteger MidiPatterngetSize(HSQUIRRELVM vm)
{
    // get "this" pointer
    SQUserPointer userPtr = 0;
    sq_getinstanceup(vm, 1, &userPtr, 0);
    Pattern *obj = static_cast<Pattern*>(userPtr);

    // call the implementation
    try {
        obj->getSize();
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // void method, returns no value
    return 0;
}

//
// Midi.Pattern print
//
SQInteger MidiPatternprint(HSQUIRRELVM vm)
{
    // get "this" pointer
    SQUserPointer userPtr = 0;
    sq_getinstanceup(vm, 1, &userPtr, 0);
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
// Midi.Tune class
//

//
// Midi.Tune getTitle
//
SQInteger MidiTunegetTitle(HSQUIRRELVM vm)
{
    // get "this" pointer
    SQUserPointer userPtr = 0;
    sq_getinstanceup(vm, 1, &userPtr, 0);
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
// Midi.Tune getTrackCount
//
SQInteger MidiTunegetTrackCount(HSQUIRRELVM vm)
{
    // get "this" pointer
    SQUserPointer userPtr = 0;
    sq_getinstanceup(vm, 1, &userPtr, 0);
    MidiTune *obj = static_cast<MidiTune*>(userPtr);

    // return value
    SQInteger ret;
    // call the implementation
    try {
        ret = obj->getTrackCount();
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // push return value
    sq_pushinteger(vm, ret);
    return 1;
}

//
// Midi.Tune getTrack
//
SQInteger MidiTunegetTrack(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs < 2) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 1");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    sq_getinstanceup(vm, 1, &userPtr, 0);
    MidiTune *obj = static_cast<MidiTune*>(userPtr);

    // get parameter 1 "number" as integer
    SQInteger number;
    if (SQ_FAILED(sq_getinteger(vm, 2, &number))){
        return sq_throwerror(vm, "argument 1 is not of type integer");
    }

    // return value
    Pattern* ret;
    // call the implementation
    try {
        ret = obj->getTrack(number);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // push return value
    sq_pushobject(vm, MidiPatternObject);
    sq_createinstance(vm, -1);
    sq_remove(vm, -2);
    sq_setinstanceup(vm, -1, ret);
    //sq_setreleasehook(vm, -1, &?);

    return 1;
}

//
// Midi.Output class
//
SQInteger MidiOutputCtor(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs < 3) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 2");
    }
    // get parameter 1 "name" as string
    const SQChar* name;
    if (SQ_FAILED(sq_getstring(vm, 2, &name))){
        return sq_throwerror(vm, "argument 1 is not of type string");
    }

    // get parameter 2 "connection" as string
    const SQChar* connection;
    if (SQ_FAILED(sq_getstring(vm, 3, &connection))){
        return sq_throwerror(vm, "argument 2 is not of type string");
    }

    MidiOutputPort *obj;
    // call the implementation
    try {
        obj = MidiOutputPortCache::instance().getMidiOutputPort(name, connection);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // return pointer to new object
    sq_setinstanceup(vm, 1, (SQUserPointer*)obj);
    //sq_setreleasehook(vm, 1, release_hook);
    return 1;
}

//
// Midi.Output setDefaultChannel
//
SQInteger MidiOutputsetDefaultChannel(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs < 2) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 1");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    sq_getinstanceup(vm, 1, &userPtr, 0);
    MidiOutputPort *obj = static_cast<MidiOutputPort*>(userPtr);

    // get parameter 1 "channel" as integer
    SQInteger channel;
    if (SQ_FAILED(sq_getinteger(vm, 2, &channel))){
        return sq_throwerror(vm, "argument 1 is not of type integer");
    }

    // call the implementation
    try {
        obj->setDefaultChannel(channel);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // void method, returns no value
    return 0;
}

//
// Midi.Output schedule
//
SQInteger MidiOutputschedule(HSQUIRRELVM vm)
{
    SQObjectType overrideType = sq_gettype(vm, 2);
    SQUserPointer overrideTypeTag;
    if(overrideType == OT_INSTANCE) {
        sq_gettypetag(vm, 2, &overrideTypeTag);
    }

    if(overrideType == OT_INSTANCE && overrideTypeTag == &MidiNoteObject) {
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs < 5) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 4");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    sq_getinstanceup(vm, 1, &userPtr, 0);
    MidiOutputPort *obj = static_cast<MidiOutputPort*>(userPtr);

    // get parameter 1 "note" as Midi.Note
    Note *note = 0;
    sq_getinstanceup(vm, 2, (SQUserPointer*)&note, 0);
    if(note == 0) {
        return sq_throwerror(vm, "argument 1 is not of type Midi.Note");
    }

    // get parameter 2 "bar" as integer
    SQInteger bar;
    if (SQ_FAILED(sq_getinteger(vm, 3, &bar))){
        return sq_throwerror(vm, "argument 2 is not of type integer");
    }

    // get parameter 3 "position" as integer
    SQInteger position;
    if (SQ_FAILED(sq_getinteger(vm, 4, &position))){
        return sq_throwerror(vm, "argument 3 is not of type integer");
    }

    // get parameter 4 "division" as integer
    SQInteger division;
    if (SQ_FAILED(sq_getinteger(vm, 5, &division))){
        return sq_throwerror(vm, "argument 4 is not of type integer");
    }

    // call the implementation
    try {
        obj->schedule(*note, bar, position, division);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // void method, returns no value
    return 0;
    }
    if(overrideType == OT_INSTANCE && overrideTypeTag == &MidiPatternObject) {
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs < 3) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 2");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    sq_getinstanceup(vm, 1, &userPtr, 0);
    MidiOutputPort *obj = static_cast<MidiOutputPort*>(userPtr);

    // get parameter 1 "pattern" as Midi.Pattern
    Pattern *pattern = 0;
    sq_getinstanceup(vm, 2, (SQUserPointer*)&pattern, 0);
    if(pattern == 0) {
        return sq_throwerror(vm, "argument 1 is not of type Midi.Pattern");
    }

    // get parameter 2 "bar" as integer
    SQInteger bar;
    if (SQ_FAILED(sq_getinteger(vm, 3, &bar))){
        return sq_throwerror(vm, "argument 2 is not of type integer");
    }

    // call the implementation
    try {
        obj->schedule(*pattern, bar);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // void method, returns no value
    return 0;
    }
    if(overrideType == OT_INSTANCE && overrideTypeTag == &MidiControlObject) {
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs < 5) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 4");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    sq_getinstanceup(vm, 1, &userPtr, 0);
    MidiOutputPort *obj = static_cast<MidiOutputPort*>(userPtr);

    // get parameter 1 "control" as Midi.Control
    Control *control = 0;
    sq_getinstanceup(vm, 2, (SQUserPointer*)&control, 0);
    if(control == 0) {
        return sq_throwerror(vm, "argument 1 is not of type Midi.Control");
    }

    // get parameter 2 "bar" as integer
    SQInteger bar;
    if (SQ_FAILED(sq_getinteger(vm, 3, &bar))){
        return sq_throwerror(vm, "argument 2 is not of type integer");
    }

    // get parameter 3 "position" as integer
    SQInteger position;
    if (SQ_FAILED(sq_getinteger(vm, 4, &position))){
        return sq_throwerror(vm, "argument 3 is not of type integer");
    }

    // get parameter 4 "division" as integer
    SQInteger division;
    if (SQ_FAILED(sq_getinteger(vm, 5, &division))){
        return sq_throwerror(vm, "argument 4 is not of type integer");
    }

    // call the implementation
    try {
        obj->schedule(*control, bar, position, division);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // void method, returns no value
    return 0;
    }
    if(overrideType == OT_INSTANCE && overrideTypeTag == &MidiProgramChangeObject) {
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs < 5) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 4");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    sq_getinstanceup(vm, 1, &userPtr, 0);
    MidiOutputPort *obj = static_cast<MidiOutputPort*>(userPtr);

    // get parameter 1 "control" as Midi.ProgramChange
    ProgramChange *control = 0;
    sq_getinstanceup(vm, 2, (SQUserPointer*)&control, 0);
    if(control == 0) {
        return sq_throwerror(vm, "argument 1 is not of type Midi.ProgramChange");
    }

    // get parameter 2 "bar" as integer
    SQInteger bar;
    if (SQ_FAILED(sq_getinteger(vm, 3, &bar))){
        return sq_throwerror(vm, "argument 2 is not of type integer");
    }

    // get parameter 3 "position" as integer
    SQInteger position;
    if (SQ_FAILED(sq_getinteger(vm, 4, &position))){
        return sq_throwerror(vm, "argument 3 is not of type integer");
    }

    // get parameter 4 "division" as integer
    SQInteger division;
    if (SQ_FAILED(sq_getinteger(vm, 5, &division))){
        return sq_throwerror(vm, "argument 4 is not of type integer");
    }

    // call the implementation
    try {
        obj->schedule(*control, bar, position, division);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // void method, returns no value
    return 0;
    }
    else {
        return sq_throwerror(vm, "argument 1 is not an expected type");
    }
}

//
// Midi.ProgramChange class
//
SQInteger MidiProgramChangeCtor(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs < 2) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 1");
    }
    // get parameter 1 "program" as integer
    SQInteger program;
    if (SQ_FAILED(sq_getinteger(vm, 2, &program))){
        return sq_throwerror(vm, "argument 1 is not of type integer");
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
    //sq_setreleasehook(vm, 1, release_hook);
    return 1;
}

//
// Midi.DrumTabReader class
//

//
// Midi.DrumTabReader read
//
SQInteger MidiDrumTabReaderread(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs < 2) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 1");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    sq_getinstanceup(vm, 1, &userPtr, 0);
    DrumTabReader *obj = static_cast<DrumTabReader*>(userPtr);

    // get parameter 1 "tab" as string
    const SQChar* tab;
    if (SQ_FAILED(sq_getstring(vm, 2, &tab))){
        return sq_throwerror(vm, "argument 1 is not of type string");
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
    //sq_setreleasehook(vm, -1, &?);

    return 1;
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

    // create class Midi.Input
    sq_pushstring(vm, "Input", -1);
    sq_newclass(vm, false);
    sq_getstackobj(vm, -1, &MidiInputObject);
    sq_settypetag(vm, -1, &MidiInputObject);

    // ctor for class Input
    sq_pushstring(vm, _SC("constructor"), -1);
    sq_newclosure(vm, &MidiInputCtor, 0);
    sq_newslot(vm, -3, false);

    // methods for class Input
    // push Input to Midi package table
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
    // push Note to Midi package table
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
    // push Control to Midi package table
    sq_newslot(vm, -3, false);

    // create class Midi.InputBuffer
    sq_pushstring(vm, "InputBuffer", -1);
    sq_newclass(vm, false);
    sq_getstackobj(vm, -1, &MidiInputBufferObject);
    sq_settypetag(vm, -1, &MidiInputBufferObject);

    // ctor for class InputBuffer
    sq_pushstring(vm, _SC("constructor"), -1);
    sq_newclosure(vm, &MidiInputBufferCtor, 0);
    sq_newslot(vm, -3, false);

    // methods for class InputBuffer
    sq_pushstring(vm, _SC("lastControlValue"), -1);
    sq_newclosure(vm, &MidiInputBufferlastControlValue, 0);
    sq_newslot(vm, -3, false);

    // push InputBuffer to Midi package table
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

    sq_pushstring(vm, _SC("getSize"), -1);
    sq_newclosure(vm, &MidiPatterngetSize, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("print"), -1);
    sq_newclosure(vm, &MidiPatternprint, 0);
    sq_newslot(vm, -3, false);

    // push Pattern to Midi package table
    sq_newslot(vm, -3, false);

    // create class Midi.Tune
    sq_pushstring(vm, "Tune", -1);
    sq_newclass(vm, false);
    sq_getstackobj(vm, -1, &MidiTuneObject);
    sq_settypetag(vm, -1, &MidiTuneObject);


    // methods for class Tune
    sq_pushstring(vm, _SC("getTitle"), -1);
    sq_newclosure(vm, &MidiTunegetTitle, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("getTrackCount"), -1);
    sq_newclosure(vm, &MidiTunegetTrackCount, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("getTrack"), -1);
    sq_newclosure(vm, &MidiTunegetTrack, 0);
    sq_newslot(vm, -3, false);

    // push Tune to Midi package table
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
    sq_pushstring(vm, _SC("setDefaultChannel"), -1);
    sq_newclosure(vm, &MidiOutputsetDefaultChannel, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("schedule"), -1);
    sq_newclosure(vm, &MidiOutputschedule, 0);
    sq_newslot(vm, -3, false);

    // push Output to Midi package table
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

    // create class Midi.DrumTabReader
    sq_pushstring(vm, "DrumTabReader", -1);
    sq_newclass(vm, false);
    sq_getstackobj(vm, -1, &MidiDrumTabReaderObject);
    sq_settypetag(vm, -1, &MidiDrumTabReaderObject);


    // methods for class DrumTabReader
    sq_pushstring(vm, _SC("read"), -1);
    sq_newclosure(vm, &MidiDrumTabReaderread, 0);
    sq_newslot(vm, -3, false);

    // push DrumTabReader to Midi package table
    sq_newslot(vm, -3, false);

    // push package "Midi" to root table
    sq_newslot(vm, -3, false);
}
}