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
#include "bindcommon.h"

#include "mixer.h"
#include "onsetdetector.h"
#include "audioport.h"
#include "audioengine.h"
#include "beattracker.h"
#include <stdexcept>
#include <cstring>

namespace binding {

// object references to types in this package
HSQOBJECT AudioMixerObject;
HSQOBJECT AudioOnsetDetectorObject;
HSQOBJECT AudioOutputObject;
HSQOBJECT AudioSystemOutObject;
HSQOBJECT AudioSystemInObject;
HSQOBJECT AudioStereoInObject;
HSQOBJECT AudioStereoOutObject;
HSQOBJECT AudioBeatTrackerObject;

//
// Audio.Mixer class
//
SQInteger AudioMixerCtor(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 3) {
        return sq_throwerror(vm, "too many parameters, expected at most 2");
    }
    if(numargs < 3) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 2");
    }
    // get parameter 1 "inputs" as integer
    SQInteger inputs;
    if (SQ_FAILED(sq_getinteger(vm, 2, &inputs))){
        return sq_throwerror(vm, "argument 1 \"inputs\" is not of type integer");
    }

    // get parameter 2 "outputs" as integer
    SQInteger outputs;
    if (SQ_FAILED(sq_getinteger(vm, 3, &outputs))){
        return sq_throwerror(vm, "argument 2 \"outputs\" is not of type integer");
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
    return 1;
}

//
// Audio.Mixer addGainController
//
SQInteger AudioMixeraddGainController(HSQUIRRELVM vm)
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
        return sq_throwerror(vm, "addGainController method needs an instance of Mixer");
    }
    Mixer *obj = static_cast<Mixer*>(userPtr);
    if(!obj) {
        return sq_throwerror(vm, "addGainController method called before Audio.Mixer constructor");
    }
    // get parameter 1 "source" as Midi.Source
    MidiSource *source = getMidiSource(vm, 2);
    if(source == 0) {
        return sq_throwerror(vm, "argument 1 \"source\" is not of type Midi.Source");
    }

    // get parameter 2 "control" as integer
    SQInteger control;
    if (SQ_FAILED(sq_getinteger(vm, 3, &control))){
        return sq_throwerror(vm, "argument 2 \"control\" is not of type integer");
    }

    // get parameter 3 "input" as integer
    SQInteger input;
    if (SQ_FAILED(sq_getinteger(vm, 4, &input))){
        return sq_throwerror(vm, "argument 3 \"input\" is not of type integer");
    }

    // get parameter 4 "output" as integer
    SQInteger output;
    if (SQ_FAILED(sq_getinteger(vm, 5, &output))){
        return sq_throwerror(vm, "argument 4 \"output\" is not of type integer");
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
// Audio.Mixer connect
//
SQInteger AudioMixerconnect(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // optional overridden parameter not here
    if(numargs < 3) {
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
            return sq_throwerror(vm, "connect method needs an instance of Mixer");
        }
        Mixer *obj = static_cast<Mixer*>(userPtr);
        if(!obj) {
            return sq_throwerror(vm, "connect method called before Audio.Mixer constructor");
        }
        // get parameter 1 "source" as Audio.Source
        AudioSource *source = getAudioSource(vm, 2);
        if(source == 0) {
            return sq_throwerror(vm, "argument 1 \"source\" is not of type Audio.Source");
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
    SQObjectType overrideType = sq_gettype(vm, 3);
    if(overrideType == OT_FLOAT) {
        SQInteger numargs = sq_gettop(vm);
        // check parameter count
        if(numargs > 3) {
            return sq_throwerror(vm, "too many parameters, expected at most 2");
        }
        if(numargs < 2) {
            return sq_throwerror(vm, "insufficient parameters, expected at least 1");
        }
        // get "this" pointer
        SQUserPointer userPtr = 0;
        if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
            return sq_throwerror(vm, "connect method needs an instance of Mixer");
        }
        Mixer *obj = static_cast<Mixer*>(userPtr);
        if(!obj) {
            return sq_throwerror(vm, "connect method called before Audio.Mixer constructor");
        }
        // get parameter 1 "source" as Audio.Source
        AudioSource *source = getAudioSource(vm, 2);
        if(source == 0) {
            return sq_throwerror(vm, "argument 1 \"source\" is not of type Audio.Source");
        }

        // 2 parameters passed in
        if(numargs == 3) {

            // get parameter 2 "gain" as float
            SQFloat gain;
            if (SQ_FAILED(sq_getfloat(vm, 3, &gain))){
                return sq_throwerror(vm, "argument 2 \"gain\" is not of type float");
            }

            // call the implementation
            try {
                obj->connect(*source, gain);
            }
            catch(std::exception const& e) {
                return sq_throwerror(vm, e.what());
            }
        }

        else {
            // call the implementation
            try {
                obj->connect(*source);
            }
            catch(std::exception const& e) {
                return sq_throwerror(vm, e.what());
            }
        }

        // void method, returns no value
        return 0;
    }
    else if(overrideType == OT_ARRAY) {
        SQInteger numargs = sq_gettop(vm);
        // check parameter count
        if(numargs > 3) {
            return sq_throwerror(vm, "too many parameters, expected at most 2");
        }
        if(numargs < 2) {
            return sq_throwerror(vm, "insufficient parameters, expected at least 1");
        }
        // get "this" pointer
        SQUserPointer userPtr = 0;
        if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
            return sq_throwerror(vm, "connect method needs an instance of Mixer");
        }
        Mixer *obj = static_cast<Mixer*>(userPtr);
        if(!obj) {
            return sq_throwerror(vm, "connect method called before Audio.Mixer constructor");
        }
        // get parameter 1 "source" as Audio.Source
        AudioSource *source = getAudioSource(vm, 2);
        if(source == 0) {
            return sq_throwerror(vm, "argument 1 \"source\" is not of type Audio.Source");
        }

        // 2 parameters passed in
        if(numargs == 3) {

            // get parameter 2 "gains" as array
            HSQOBJECT gainsObj;
            if (SQ_FAILED(sq_getstackobj(vm, 3, &gainsObj))) {
                return sq_throwerror(vm, "argument 2 \"gains\" is not of type array");
            }
            if (sq_gettype(vm, 3) != OT_ARRAY) {
                return sq_throwerror(vm, "argument 2 \"gains\" is not of type array");
            }
            ScriptArray gains(vm, gainsObj);

            // call the implementation
            try {
                obj->connect(*source, gains);
            }
            catch(std::exception const& e) {
                return sq_throwerror(vm, e.what());
            }
        }

        else {
            // call the implementation
            try {
                obj->connect(*source);
            }
            catch(std::exception const& e) {
                return sq_throwerror(vm, e.what());
            }
        }

        // void method, returns no value
        return 0;
    }
    else {
        return sq_throwerror(vm, "argument 2 is not of type {float, array}");
    }
}

//
// Audio.Mixer output
//
SQInteger AudioMixeroutput(HSQUIRRELVM vm)
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
        return sq_throwerror(vm, "output method needs an instance of Mixer");
    }
    Mixer *obj = static_cast<Mixer*>(userPtr);
    if(!obj) {
        return sq_throwerror(vm, "output method called before Audio.Mixer constructor");
    }
    // get parameter 1 "channel" as integer
    SQInteger channel;
    if (SQ_FAILED(sq_getinteger(vm, 2, &channel))){
        return sq_throwerror(vm, "argument 1 \"channel\" is not of type integer");
    }

    // return value
    AudioConnection* ret;
    // call the implementation
    try {
        ret = obj->getAudioConnection(channel);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // push return value
    sq_pushobject(vm, AudioOutputObject);
    sq_createinstance(vm, -1);
    sq_remove(vm, -2);
    sq_setinstanceup(vm, -1, ret);
    // no release hook, release ignored per binding

    return 1;
}

//
// Audio.Mixer scheduleGain
//
SQInteger AudioMixerscheduleGain(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 7) {
        return sq_throwerror(vm, "too many parameters, expected at most 6");
    }
    if(numargs < 7) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 6");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "scheduleGain method needs an instance of Mixer");
    }
    Mixer *obj = static_cast<Mixer*>(userPtr);
    if(!obj) {
        return sq_throwerror(vm, "scheduleGain method called before Audio.Mixer constructor");
    }
    // get parameter 1 "input" as integer
    SQInteger input;
    if (SQ_FAILED(sq_getinteger(vm, 2, &input))){
        return sq_throwerror(vm, "argument 1 \"input\" is not of type integer");
    }

    // get parameter 2 "output" as integer
    SQInteger output;
    if (SQ_FAILED(sq_getinteger(vm, 3, &output))){
        return sq_throwerror(vm, "argument 2 \"output\" is not of type integer");
    }

    // get parameter 3 "gain" as float
    SQFloat gain;
    if (SQ_FAILED(sq_getfloat(vm, 4, &gain))){
        return sq_throwerror(vm, "argument 3 \"gain\" is not of type float");
    }

    // get parameter 4 "bar" as integer
    SQInteger bar;
    if (SQ_FAILED(sq_getinteger(vm, 5, &bar))){
        return sq_throwerror(vm, "argument 4 \"bar\" is not of type integer");
    }

    // get parameter 5 "position" as integer
    SQInteger position;
    if (SQ_FAILED(sq_getinteger(vm, 6, &position))){
        return sq_throwerror(vm, "argument 5 \"position\" is not of type integer");
    }

    // get parameter 6 "division" as integer
    SQInteger division;
    if (SQ_FAILED(sq_getinteger(vm, 7, &division))){
        return sq_throwerror(vm, "argument 6 \"division\" is not of type integer");
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
// Audio.OnsetDetector class
//
SQInteger AudioOnsetDetectorCtor(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 1) {
        return sq_throwerror(vm, "too many parameters, expected at most 0");
    }
    OnsetDetector *obj;
    // call the implementation
    try {
        obj = OnsetDetectorCache::instance().getOnsetDetector();
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // return pointer to new object
    sq_setinstanceup(vm, 1, (SQUserPointer*)obj);
    return 1;
}

//
// Audio.OnsetDetector connect
//
SQInteger AudioOnsetDetectorconnect(HSQUIRRELVM vm)
{
    SQObjectType overrideType = sq_gettype(vm, 2);
    if(AudioSource *source = getAudioSource(vm, 2)) {
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
            return sq_throwerror(vm, "connect method needs an instance of OnsetDetector");
        }
        OnsetDetector *obj = static_cast<OnsetDetector*>(userPtr);
        if(!obj) {
            return sq_throwerror(vm, "connect method called before Audio.OnsetDetector constructor");
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
    else if(AudioConnection *output = getAudioOutput(vm, 2)) {
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
            return sq_throwerror(vm, "connect method needs an instance of OnsetDetector");
        }
        OnsetDetector *obj = static_cast<OnsetDetector*>(userPtr);
        if(!obj) {
            return sq_throwerror(vm, "connect method called before Audio.OnsetDetector constructor");
        }
        // call the implementation
        try {
            obj->connect(*output);
        }
        catch(std::exception const& e) {
            return sq_throwerror(vm, e.what());
        }

        // void method, returns no value
        return 0;
    }
    else {
        return sq_throwerror(vm, "argument 1 is not of type {Audio.Source, Audio.Output}");
    }
}

//
// Audio.OnsetDetector onOnset
//
SQInteger AudioOnsetDetectoronOnset(HSQUIRRELVM vm)
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
        return sq_throwerror(vm, "onOnset method needs an instance of OnsetDetector");
    }
    OnsetDetector *obj = static_cast<OnsetDetector*>(userPtr);
    if(!obj) {
        return sq_throwerror(vm, "onOnset method called before Audio.OnsetDetector constructor");
    }
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
        obj->onOnset(handler);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // void method, returns no value
    return 0;
}

//
// Audio.OnsetDetector threshold
//
SQInteger AudioOnsetDetectorthreshold(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 2) {
        return sq_throwerror(vm, "too many parameters, expected at most 1");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "threshold method needs an instance of OnsetDetector");
    }
    OnsetDetector *obj = static_cast<OnsetDetector*>(userPtr);
    if(!obj) {
        return sq_throwerror(vm, "threshold method called before Audio.OnsetDetector constructor");
    }
    // return value
    SQFloat ret;
    // 1 parameters passed in
    if(numargs == 2) {

        // get parameter 1 "threshold" as float
        SQFloat threshold;
        if (SQ_FAILED(sq_getfloat(vm, 2, &threshold))){
            return sq_throwerror(vm, "argument 1 \"threshold\" is not of type float");
        }

        // call the implementation
        try {
            ret = obj->threshold(threshold);
        }
        catch(std::exception const& e) {
            return sq_throwerror(vm, e.what());
        }
    }

    else {
        // call the implementation
        try {
            ret = obj->threshold();
        }
        catch(std::exception const& e) {
            return sq_throwerror(vm, e.what());
        }
    }

    // push return value
    sq_pushfloat(vm, ret);
    return 1;
}

//
// Audio.Output class
//
AudioConnection *getAudioOutput(HSQUIRRELVM &vm, int index) {
    SQUserPointer objPtr;
    if (!SQ_FAILED(sq_getinstanceup(vm, index, (SQUserPointer*)&objPtr, &AudioOutputObject))) {
        return static_cast<AudioConnection*>(objPtr);
    }
}

SQInteger AudioOutputCtor(HSQUIRRELVM vm)
{
    return sq_throwerror(vm, "cannot directly instantiate Audio.Output");
}

//
// Audio.SystemOut class
//
SQInteger AudioSystemOutCtor(HSQUIRRELVM vm)
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

    AudioOutputPort *obj;
    // 2 parameters passed in
    if(numargs == 3) {

        // get parameter 2 "connection" as string
        const SQChar* connection;
        if (SQ_FAILED(sq_getstring(vm, 3, &connection))){
            return sq_throwerror(vm, "argument 2 \"connection\" is not of type string");
        }

        // call the implementation
        try {
            obj = AudioOutputPortCache::instance().getAudioOutputPort(name, connection);
        }
        catch(std::exception const& e) {
            return sq_throwerror(vm, e.what());
        }
    }

    else {
        // call the implementation
        try {
            obj = AudioOutputPortCache::instance().getAudioOutputPort(name);
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
// Audio.SystemOut connect
//
SQInteger AudioSystemOutconnect(HSQUIRRELVM vm)
{
    SQObjectType overrideType = sq_gettype(vm, 2);
    if(AudioSource *source = getAudioSource(vm, 2)) {
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
            return sq_throwerror(vm, "connect method needs an instance of SystemOut");
        }
        AudioOutputPort *obj = static_cast<AudioOutputPort*>(userPtr);
        if(!obj) {
            return sq_throwerror(vm, "connect method called before Audio.SystemOut constructor");
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
    else if(AudioConnection *output = getAudioOutput(vm, 2)) {
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
            return sq_throwerror(vm, "connect method needs an instance of SystemOut");
        }
        AudioOutputPort *obj = static_cast<AudioOutputPort*>(userPtr);
        if(!obj) {
            return sq_throwerror(vm, "connect method called before Audio.SystemOut constructor");
        }
        // call the implementation
        try {
            obj->connect(*output);
        }
        catch(std::exception const& e) {
            return sq_throwerror(vm, e.what());
        }

        // void method, returns no value
        return 0;
    }
    else {
        return sq_throwerror(vm, "argument 1 is not of type {Audio.Source, Audio.Output}");
    }
}

//
// Audio.SystemIn class
//
SQInteger AudioSystemInCtor(HSQUIRRELVM vm)
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

    AudioInputPort *obj;
    // 2 parameters passed in
    if(numargs == 3) {

        // get parameter 2 "connection" as string
        const SQChar* connection;
        if (SQ_FAILED(sq_getstring(vm, 3, &connection))){
            return sq_throwerror(vm, "argument 2 \"connection\" is not of type string");
        }

        // call the implementation
        try {
            obj = AudioInputPortCache::instance().getAudioInputPort(name, connection);
        }
        catch(std::exception const& e) {
            return sq_throwerror(vm, e.what());
        }
    }

    else {
        // call the implementation
        try {
            obj = AudioInputPortCache::instance().getAudioInputPort(name);
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
// Audio.SystemIn output
//
SQInteger AudioSystemInoutput(HSQUIRRELVM vm)
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
        return sq_throwerror(vm, "output method needs an instance of SystemIn");
    }
    AudioInputPort *obj = static_cast<AudioInputPort*>(userPtr);
    if(!obj) {
        return sq_throwerror(vm, "output method called before Audio.SystemIn constructor");
    }
    // get parameter 1 "channel" as integer
    SQInteger channel;
    if (SQ_FAILED(sq_getinteger(vm, 2, &channel))){
        return sq_throwerror(vm, "argument 1 \"channel\" is not of type integer");
    }

    // return value
    AudioConnection* ret;
    // call the implementation
    try {
        ret = obj->getAudioConnection(channel);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // push return value
    sq_pushobject(vm, AudioOutputObject);
    sq_createinstance(vm, -1);
    sq_remove(vm, -2);
    sq_setinstanceup(vm, -1, ret);
    // no release hook, release ignored per binding

    return 1;
}

//
// Audio.StereoIn class
//
SQInteger AudioStereoInCtor(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 4) {
        return sq_throwerror(vm, "too many parameters, expected at most 3");
    }
    if(numargs < 4) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 3");
    }
    // get parameter 1 "name" as string
    const SQChar* name;
    if (SQ_FAILED(sq_getstring(vm, 2, &name))){
        return sq_throwerror(vm, "argument 1 \"name\" is not of type string");
    }

    // get parameter 2 "connectLeft" as string
    const SQChar* connectLeft;
    if (SQ_FAILED(sq_getstring(vm, 3, &connectLeft))){
        return sq_throwerror(vm, "argument 2 \"connectLeft\" is not of type string");
    }

    // get parameter 3 "connectRight" as string
    const SQChar* connectRight;
    if (SQ_FAILED(sq_getstring(vm, 4, &connectRight))){
        return sq_throwerror(vm, "argument 3 \"connectRight\" is not of type string");
    }

    AudioStereoInput *obj;
    // call the implementation
    try {
        obj = AudioStereoInputCache::instance().getAudioStereoInput(name, connectLeft, connectRight);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // return pointer to new object
    sq_setinstanceup(vm, 1, (SQUserPointer*)obj);
    return 1;
}

//
// Audio.StereoIn output
//
SQInteger AudioStereoInoutput(HSQUIRRELVM vm)
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
        return sq_throwerror(vm, "output method needs an instance of StereoIn");
    }
    AudioStereoInput *obj = static_cast<AudioStereoInput*>(userPtr);
    if(!obj) {
        return sq_throwerror(vm, "output method called before Audio.StereoIn constructor");
    }
    // get parameter 1 "channel" as integer
    SQInteger channel;
    if (SQ_FAILED(sq_getinteger(vm, 2, &channel))){
        return sq_throwerror(vm, "argument 1 \"channel\" is not of type integer");
    }

    // return value
    AudioConnection* ret;
    // call the implementation
    try {
        ret = obj->getAudioConnection(channel);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // push return value
    sq_pushobject(vm, AudioOutputObject);
    sq_createinstance(vm, -1);
    sq_remove(vm, -2);
    sq_setinstanceup(vm, -1, ret);
    // no release hook, release ignored per binding

    return 1;
}

//
// Audio.StereoOut class
//
SQInteger AudioStereoOutRelease(SQUserPointer p, SQInteger size)
{
    delete static_cast<AudioStereoOutput*>(p);
}

SQInteger AudioStereoOutCtor(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 4) {
        return sq_throwerror(vm, "too many parameters, expected at most 3");
    }
    if(numargs < 4) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 3");
    }
    // get parameter 1 "name" as string
    const SQChar* name;
    if (SQ_FAILED(sq_getstring(vm, 2, &name))){
        return sq_throwerror(vm, "argument 1 \"name\" is not of type string");
    }

    // get parameter 2 "connectLeft" as string
    const SQChar* connectLeft;
    if (SQ_FAILED(sq_getstring(vm, 3, &connectLeft))){
        return sq_throwerror(vm, "argument 2 \"connectLeft\" is not of type string");
    }

    // get parameter 3 "connectRight" as string
    const SQChar* connectRight;
    if (SQ_FAILED(sq_getstring(vm, 4, &connectRight))){
        return sq_throwerror(vm, "argument 3 \"connectRight\" is not of type string");
    }

    AudioStereoOutput *obj;
    // call the implementation
    try {
        obj = new AudioStereoOutput(name, connectLeft, connectRight);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // return pointer to new object
    sq_setinstanceup(vm, 1, (SQUserPointer*)obj);
    sq_setreleasehook(vm, 1, AudioStereoOutRelease);
    return 1;
}

SQInteger AudioStereoOutClone(HSQUIRRELVM vm)
{
    // get instance ptr of original
    SQUserPointer userPtr;
    sq_getinstanceup(vm, 2, &userPtr, 0);
    // set instance ptr to a copy
    sq_setinstanceup(vm, 1, new AudioStereoOutput(*(AudioStereoOutput*)userPtr));
    sq_setreleasehook(vm, 1, &AudioStereoOutRelease);
    return 0;
}

//
// Audio.StereoOut connect
//
SQInteger AudioStereoOutconnect(HSQUIRRELVM vm)
{
    SQObjectType overrideType = sq_gettype(vm, 2);
    if(AudioSource *source = getAudioSource(vm, 2)) {
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
            return sq_throwerror(vm, "connect method needs an instance of StereoOut");
        }
        AudioStereoOutput *obj = static_cast<AudioStereoOutput*>(userPtr);
        if(!obj) {
            return sq_throwerror(vm, "connect method called before Audio.StereoOut constructor");
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
    else if(AudioConnection *output = getAudioOutput(vm, 2)) {
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
            return sq_throwerror(vm, "connect method needs an instance of StereoOut");
        }
        AudioStereoOutput *obj = static_cast<AudioStereoOutput*>(userPtr);
        if(!obj) {
            return sq_throwerror(vm, "connect method called before Audio.StereoOut constructor");
        }
        // call the implementation
        try {
            obj->connect(*output);
        }
        catch(std::exception const& e) {
            return sq_throwerror(vm, e.what());
        }

        // void method, returns no value
        return 0;
    }
    else {
        return sq_throwerror(vm, "argument 1 is not of type {Audio.Source, Audio.Output}");
    }
}

//
// Audio.BeatTracker class
//
SQInteger AudioBeatTrackerCtor(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 2) {
        return sq_throwerror(vm, "too many parameters, expected at most 1");
    }
    if(numargs < 2) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 1");
    }
    // get parameter 1 "bpm" as float
    SQFloat bpm;
    if (SQ_FAILED(sq_getfloat(vm, 2, &bpm))){
        return sq_throwerror(vm, "argument 1 \"bpm\" is not of type float");
    }

    BeatTracker *obj;
    // call the implementation
    try {
        obj = BeatTrackerCache::instance().getBeatTracker(bpm);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // return pointer to new object
    sq_setinstanceup(vm, 1, (SQUserPointer*)obj);
    return 1;
}

//
// Audio.BeatTracker connect
//
SQInteger AudioBeatTrackerconnect(HSQUIRRELVM vm)
{
    SQObjectType overrideType = sq_gettype(vm, 2);
    if(AudioSource *source = getAudioSource(vm, 2)) {
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
            return sq_throwerror(vm, "connect method needs an instance of BeatTracker");
        }
        BeatTracker *obj = static_cast<BeatTracker*>(userPtr);
        if(!obj) {
            return sq_throwerror(vm, "connect method called before Audio.BeatTracker constructor");
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
    else if(AudioConnection *output = getAudioOutput(vm, 2)) {
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
            return sq_throwerror(vm, "connect method needs an instance of BeatTracker");
        }
        BeatTracker *obj = static_cast<BeatTracker*>(userPtr);
        if(!obj) {
            return sq_throwerror(vm, "connect method called before Audio.BeatTracker constructor");
        }
        // call the implementation
        try {
            obj->connect(*output);
        }
        catch(std::exception const& e) {
            return sq_throwerror(vm, e.what());
        }

        // void method, returns no value
        return 0;
    }
    else {
        return sq_throwerror(vm, "argument 1 is not of type {Audio.Source, Audio.Output}");
    }
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

    // clone for class Mixer
    sq_pushstring(vm, _SC("_cloned"), -1);
    sq_newclosure(vm, &unclonable, 0);
    sq_newslot(vm, -3, false);

    // methods for class Mixer
    sq_pushstring(vm, _SC("addGainController"), -1);
    sq_newclosure(vm, &AudioMixeraddGainController, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("connect"), -1);
    sq_newclosure(vm, &AudioMixerconnect, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("output"), -1);
    sq_newclosure(vm, &AudioMixeroutput, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("scheduleGain"), -1);
    sq_newclosure(vm, &AudioMixerscheduleGain, 0);
    sq_newslot(vm, -3, false);

    // push Mixer to Audio package table
    sq_newslot(vm, -3, false);

    // create class Audio.OnsetDetector
    sq_pushstring(vm, "OnsetDetector", -1);
    sq_newclass(vm, false);
    sq_getstackobj(vm, -1, &AudioOnsetDetectorObject);
    sq_settypetag(vm, -1, &AudioOnsetDetectorObject);

    // ctor for class OnsetDetector
    sq_pushstring(vm, _SC("constructor"), -1);
    sq_newclosure(vm, &AudioOnsetDetectorCtor, 0);
    sq_newslot(vm, -3, false);

    // clone for class OnsetDetector
    sq_pushstring(vm, _SC("_cloned"), -1);
    sq_newclosure(vm, &unclonable, 0);
    sq_newslot(vm, -3, false);

    // methods for class OnsetDetector
    sq_pushstring(vm, _SC("connect"), -1);
    sq_newclosure(vm, &AudioOnsetDetectorconnect, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("onOnset"), -1);
    sq_newclosure(vm, &AudioOnsetDetectoronOnset, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("threshold"), -1);
    sq_newclosure(vm, &AudioOnsetDetectorthreshold, 0);
    sq_newslot(vm, -3, false);

    // push OnsetDetector to Audio package table
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

    // clone for class Output
    sq_pushstring(vm, _SC("_cloned"), -1);
    sq_newclosure(vm, &unclonable, 0);
    sq_newslot(vm, -3, false);

    // methods for class Output
    // push Output to Audio package table
    sq_newslot(vm, -3, false);

    // create class Audio.SystemOut
    sq_pushstring(vm, "SystemOut", -1);
    sq_newclass(vm, false);
    sq_getstackobj(vm, -1, &AudioSystemOutObject);
    sq_settypetag(vm, -1, &AudioSystemOutObject);

    // ctor for class SystemOut
    sq_pushstring(vm, _SC("constructor"), -1);
    sq_newclosure(vm, &AudioSystemOutCtor, 0);
    sq_newslot(vm, -3, false);

    // clone for class SystemOut
    sq_pushstring(vm, _SC("_cloned"), -1);
    sq_newclosure(vm, &unclonable, 0);
    sq_newslot(vm, -3, false);

    // methods for class SystemOut
    sq_pushstring(vm, _SC("connect"), -1);
    sq_newclosure(vm, &AudioSystemOutconnect, 0);
    sq_newslot(vm, -3, false);

    // push SystemOut to Audio package table
    sq_newslot(vm, -3, false);

    // create class Audio.SystemIn
    sq_pushstring(vm, "SystemIn", -1);
    sq_newclass(vm, false);
    sq_getstackobj(vm, -1, &AudioSystemInObject);
    sq_settypetag(vm, -1, &AudioSystemInObject);

    // ctor for class SystemIn
    sq_pushstring(vm, _SC("constructor"), -1);
    sq_newclosure(vm, &AudioSystemInCtor, 0);
    sq_newslot(vm, -3, false);

    // clone for class SystemIn
    sq_pushstring(vm, _SC("_cloned"), -1);
    sq_newclosure(vm, &unclonable, 0);
    sq_newslot(vm, -3, false);

    // methods for class SystemIn
    sq_pushstring(vm, _SC("output"), -1);
    sq_newclosure(vm, &AudioSystemInoutput, 0);
    sq_newslot(vm, -3, false);

    // push SystemIn to Audio package table
    sq_newslot(vm, -3, false);

    // create class Audio.StereoIn
    sq_pushstring(vm, "StereoIn", -1);
    sq_newclass(vm, false);
    sq_getstackobj(vm, -1, &AudioStereoInObject);
    sq_settypetag(vm, -1, &AudioStereoInObject);

    // ctor for class StereoIn
    sq_pushstring(vm, _SC("constructor"), -1);
    sq_newclosure(vm, &AudioStereoInCtor, 0);
    sq_newslot(vm, -3, false);

    // clone for class StereoIn
    sq_pushstring(vm, _SC("_cloned"), -1);
    sq_newclosure(vm, &unclonable, 0);
    sq_newslot(vm, -3, false);

    // methods for class StereoIn
    sq_pushstring(vm, _SC("output"), -1);
    sq_newclosure(vm, &AudioStereoInoutput, 0);
    sq_newslot(vm, -3, false);

    // push StereoIn to Audio package table
    sq_newslot(vm, -3, false);

    // create class Audio.StereoOut
    sq_pushstring(vm, "StereoOut", -1);
    sq_newclass(vm, false);
    sq_getstackobj(vm, -1, &AudioStereoOutObject);
    sq_settypetag(vm, -1, &AudioStereoOutObject);

    // ctor for class StereoOut
    sq_pushstring(vm, _SC("constructor"), -1);
    sq_newclosure(vm, &AudioStereoOutCtor, 0);
    sq_newslot(vm, -3, false);

    // clone for class StereoOut
    sq_pushstring(vm, _SC("_cloned"), -1);
    sq_newclosure(vm, &AudioStereoOutClone, 0);
    sq_newslot(vm, -3, false);

    // methods for class StereoOut
    sq_pushstring(vm, _SC("connect"), -1);
    sq_newclosure(vm, &AudioStereoOutconnect, 0);
    sq_newslot(vm, -3, false);

    // push StereoOut to Audio package table
    sq_newslot(vm, -3, false);

    // create class Audio.BeatTracker
    sq_pushstring(vm, "BeatTracker", -1);
    sq_newclass(vm, false);
    sq_getstackobj(vm, -1, &AudioBeatTrackerObject);
    sq_settypetag(vm, -1, &AudioBeatTrackerObject);

    // ctor for class BeatTracker
    sq_pushstring(vm, _SC("constructor"), -1);
    sq_newclosure(vm, &AudioBeatTrackerCtor, 0);
    sq_newslot(vm, -3, false);

    // clone for class BeatTracker
    sq_pushstring(vm, _SC("_cloned"), -1);
    sq_newclosure(vm, &unclonable, 0);
    sq_newslot(vm, -3, false);

    // methods for class BeatTracker
    sq_pushstring(vm, _SC("connect"), -1);
    sq_newclosure(vm, &AudioBeatTrackerconnect, 0);
    sq_newslot(vm, -3, false);

    // push BeatTracker to Audio package table
    sq_newslot(vm, -3, false);

    // push package "Audio" to root table
    sq_newslot(vm, -3, false);
}
}