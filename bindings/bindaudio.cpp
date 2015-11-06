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

#include "bindaudio.h"
#include "bindtypes.h"
#include "bindings.h"

#include "mixer.h"
#include "audioport.h"
#include "audioengine.h"
#include "stereoport.h"

namespace binding {

// object references to types in this package
HSQOBJECT AudioMixerObject;
HSQOBJECT AudioOutputObject;
HSQOBJECT AudioInputObject;
HSQOBJECT AudioStereoOutputObject;

//
// Audio.Mixer class
//
SQInteger AudioMixerCtor(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs < 3) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 2");
    }
    // get parameter 1 "inputs" as integer
    SQInteger inputs;
    if (SQ_FAILED(sq_getinteger(vm, 2, &inputs))){
        return sq_throwerror(vm, "argument 1 is not of type integer");
    }

    // get parameter 2 "outputs" as integer
    SQInteger outputs;
    if (SQ_FAILED(sq_getinteger(vm, 3, &outputs))){
        return sq_throwerror(vm, "argument 2 is not of type integer");
    }

    Mixer *obj;
    // call the implementation
    try {
        obj = MixerCache::instance().getMixer(inputs, outputs);
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
// Audio.Mixer addGainController
//
SQInteger AudioMixeraddGainController(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs < 5) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 4");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    sq_getinstanceup(vm, 1, &userPtr, 0);
    Mixer *obj = static_cast<Mixer*>(userPtr);

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

    // get parameter 2 "control" as integer
    SQInteger control;
    if (SQ_FAILED(sq_getinteger(vm, 3, &control))){
        return sq_throwerror(vm, "argument 2 is not of type integer");
    }

    // get parameter 3 "input" as integer
    SQInteger input;
    if (SQ_FAILED(sq_getinteger(vm, 4, &input))){
        return sq_throwerror(vm, "argument 3 is not of type integer");
    }

    // get parameter 4 "output" as integer
    SQInteger output;
    if (SQ_FAILED(sq_getinteger(vm, 5, &output))){
        return sq_throwerror(vm, "argument 4 is not of type integer");
    }

    // call the implementation
    try {
        obj->addGainController(*source, control, input, output);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // void method, returns no value
    return 0;
}

//
// Audio.Mixer nextAudioInput
//
SQInteger AudioMixernextAudioInput(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs < 2) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 1");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    sq_getinstanceup(vm, 1, &userPtr, 0);
    Mixer *obj = static_cast<Mixer*>(userPtr);

    // get parameter 1 "source" as AudioSource
    SQUserPointer sourceTypeTag, sourcePtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 2, (SQUserPointer*)&sourcePtr, 0))) {
        return sq_throwerror(vm, "argument 1 is not an object of type AudioSource");
    }
    sq_gettypetag(vm, 2, &sourceTypeTag);
    AudioSource *source = getAudioSource(sourcePtr, sourceTypeTag);
    if(source == 0) {
        return sq_throwerror(vm, "argument 1 is not of type AudioSource");
    }

    // call the implementation
    try {
        obj->nextAudioInput(*source);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // void method, returns no value
    return 0;
}

//
// Audio.Mixer nextStereoInput
//
SQInteger AudioMixernextStereoInput(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs < 2) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 1");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    sq_getinstanceup(vm, 1, &userPtr, 0);
    Mixer *obj = static_cast<Mixer*>(userPtr);

    // get parameter 1 "source" as AudioSource
    SQUserPointer sourceTypeTag, sourcePtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 2, (SQUserPointer*)&sourcePtr, 0))) {
        return sq_throwerror(vm, "argument 1 is not an object of type AudioSource");
    }
    sq_gettypetag(vm, 2, &sourceTypeTag);
    AudioSource *source = getAudioSource(sourcePtr, sourceTypeTag);
    if(source == 0) {
        return sq_throwerror(vm, "argument 1 is not of type AudioSource");
    }

    // call the implementation
    try {
        obj->nextStereoInput(*source);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // void method, returns no value
    return 0;
}

//
// Audio.Mixer scheduleGain
//
SQInteger AudioMixerscheduleGain(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs < 7) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 6");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    sq_getinstanceup(vm, 1, &userPtr, 0);
    Mixer *obj = static_cast<Mixer*>(userPtr);

    // get parameter 1 "input" as integer
    SQInteger input;
    if (SQ_FAILED(sq_getinteger(vm, 2, &input))){
        return sq_throwerror(vm, "argument 1 is not of type integer");
    }

    // get parameter 2 "output" as integer
    SQInteger output;
    if (SQ_FAILED(sq_getinteger(vm, 3, &output))){
        return sq_throwerror(vm, "argument 2 is not of type integer");
    }

    // get parameter 3 "gain" as float
    SQFloat gain;
    if (SQ_FAILED(sq_getfloat(vm, 4, &gain))){
        return sq_throwerror(vm, "argument 3 is not of type float");
    }

    // get parameter 4 "bar" as integer
    SQInteger bar;
    if (SQ_FAILED(sq_getinteger(vm, 5, &bar))){
        return sq_throwerror(vm, "argument 4 is not of type integer");
    }

    // get parameter 5 "position" as integer
    SQInteger position;
    if (SQ_FAILED(sq_getinteger(vm, 6, &position))){
        return sq_throwerror(vm, "argument 5 is not of type integer");
    }

    // get parameter 6 "division" as integer
    SQInteger division;
    if (SQ_FAILED(sq_getinteger(vm, 7, &division))){
        return sq_throwerror(vm, "argument 6 is not of type integer");
    }

    // call the implementation
    try {
        obj->scheduleGain(input, output, gain, bar, position, division);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // void method, returns no value
    return 0;
}

//
// Audio.Output class
//
SQInteger AudioOutputCtor(HSQUIRRELVM vm)
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

    AudioOutputPort *obj;
    // call the implementation
    try {
        obj = AudioOutputPortCache::instance().getAudioOutputPort(name, connection);
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
// Audio.Output connect
//
SQInteger AudioOutputconnect(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs < 2) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 1");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    sq_getinstanceup(vm, 1, &userPtr, 0);
    AudioOutputPort *obj = static_cast<AudioOutputPort*>(userPtr);

    // get parameter 1 "source" as AudioSource
    SQUserPointer sourceTypeTag, sourcePtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 2, (SQUserPointer*)&sourcePtr, 0))) {
        return sq_throwerror(vm, "argument 1 is not an object of type AudioSource");
    }
    sq_gettypetag(vm, 2, &sourceTypeTag);
    AudioSource *source = getAudioSource(sourcePtr, sourceTypeTag);
    if(source == 0) {
        return sq_throwerror(vm, "argument 1 is not of type AudioSource");
    }

    // call the implementation
    try {
        obj->connect(*source);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // void method, returns no value
    return 0;
}

//
// Audio.Input class
//
SQInteger AudioInputCtor(HSQUIRRELVM vm)
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

    AudioInputPort *obj;
    // call the implementation
    try {
        obj = AudioInputPortCache::instance().getAudioInputPort(name, connection);
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
// Audio.StereoOutput class
//
SQInteger AudioStereoOutputCtor(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs < 4) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 3");
    }
    // get parameter 1 "name" as string
    const SQChar* name;
    if (SQ_FAILED(sq_getstring(vm, 2, &name))){
        return sq_throwerror(vm, "argument 1 is not of type string");
    }

    // get parameter 2 "connectLeft" as string
    const SQChar* connectLeft;
    if (SQ_FAILED(sq_getstring(vm, 3, &connectLeft))){
        return sq_throwerror(vm, "argument 2 is not of type string");
    }

    // get parameter 3 "connectRight" as string
    const SQChar* connectRight;
    if (SQ_FAILED(sq_getstring(vm, 4, &connectRight))){
        return sq_throwerror(vm, "argument 3 is not of type string");
    }

    StereoOutput *obj;
    // call the implementation
    try {
        obj = new StereoOutput(name, connectLeft, connectRight);
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
// Audio.StereoOutput connect
//
SQInteger AudioStereoOutputconnect(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs < 2) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 1");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    sq_getinstanceup(vm, 1, &userPtr, 0);
    StereoOutput *obj = static_cast<StereoOutput*>(userPtr);

    // get parameter 1 "source" as AudioSource
    SQUserPointer sourceTypeTag, sourcePtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 2, (SQUserPointer*)&sourcePtr, 0))) {
        return sq_throwerror(vm, "argument 1 is not an object of type AudioSource");
    }
    sq_gettypetag(vm, 2, &sourceTypeTag);
    AudioSource *source = getAudioSource(sourcePtr, sourceTypeTag);
    if(source == 0) {
        return sq_throwerror(vm, "argument 1 is not of type AudioSource");
    }

    // call the implementation
    try {
        obj->connect(*source);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // void method, returns no value
    return 0;
}


void bindAudio(HSQUIRRELVM vm)
{
    // create package table
    sq_pushstring(vm, "Audio", -1);
    sq_newtable(vm);

    // create class Audio.Mixer
    sq_pushstring(vm, "Mixer", -1);
    sq_newclass(vm, false);
    sq_getstackobj(vm, -1, &AudioMixerObject);
    sq_settypetag(vm, -1, &AudioMixerObject);

    // ctor for class Mixer
    sq_pushstring(vm, _SC("constructor"), -1);
    sq_newclosure(vm, &AudioMixerCtor, 0);
    sq_newslot(vm, -3, false);

    // methods for class Mixer
    sq_pushstring(vm, _SC("addGainController"), -1);
    sq_newclosure(vm, &AudioMixeraddGainController, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("nextAudioInput"), -1);
    sq_newclosure(vm, &AudioMixernextAudioInput, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("nextStereoInput"), -1);
    sq_newclosure(vm, &AudioMixernextStereoInput, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("scheduleGain"), -1);
    sq_newclosure(vm, &AudioMixerscheduleGain, 0);
    sq_newslot(vm, -3, false);

    // push Mixer to Audio package table
    sq_newslot(vm, -3, false);

    // create class Audio.Output
    sq_pushstring(vm, "Output", -1);
    sq_newclass(vm, false);
    sq_getstackobj(vm, -1, &AudioOutputObject);
    sq_settypetag(vm, -1, &AudioOutputObject);

    // ctor for class Output
    sq_pushstring(vm, _SC("constructor"), -1);
    sq_newclosure(vm, &AudioOutputCtor, 0);
    sq_newslot(vm, -3, false);

    // methods for class Output
    sq_pushstring(vm, _SC("connect"), -1);
    sq_newclosure(vm, &AudioOutputconnect, 0);
    sq_newslot(vm, -3, false);

    // push Output to Audio package table
    sq_newslot(vm, -3, false);

    // create class Audio.Input
    sq_pushstring(vm, "Input", -1);
    sq_newclass(vm, false);
    sq_getstackobj(vm, -1, &AudioInputObject);
    sq_settypetag(vm, -1, &AudioInputObject);

    // ctor for class Input
    sq_pushstring(vm, _SC("constructor"), -1);
    sq_newclosure(vm, &AudioInputCtor, 0);
    sq_newslot(vm, -3, false);

    // methods for class Input
    // push Input to Audio package table
    sq_newslot(vm, -3, false);

    // create class Audio.StereoOutput
    sq_pushstring(vm, "StereoOutput", -1);
    sq_newclass(vm, false);
    sq_getstackobj(vm, -1, &AudioStereoOutputObject);
    sq_settypetag(vm, -1, &AudioStereoOutputObject);

    // ctor for class StereoOutput
    sq_pushstring(vm, _SC("constructor"), -1);
    sq_newclosure(vm, &AudioStereoOutputCtor, 0);
    sq_newslot(vm, -3, false);

    // methods for class StereoOutput
    sq_pushstring(vm, _SC("connect"), -1);
    sq_newclosure(vm, &AudioStereoOutputconnect, 0);
    sq_newslot(vm, -3, false);

    // push StereoOutput to Audio package table
    sq_newslot(vm, -3, false);

    // push package "Audio" to root table
    sq_newslot(vm, -3, false);
}
}