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

#include "bindlv2.h"
#include "bindtypes.h"
#include "bindaudio.h"
#include "bindmidi.h"

#include "lv2plugin.h"

namespace binding {

// object references to types in this package
HSQOBJECT Lv2PluginObject;
HSQOBJECT Lv2StateObject;

//
// Lv2.Plugin class
//
SQInteger Lv2PluginCtor(HSQUIRRELVM vm)
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
        // get parameter 1 "uri" as string
        const SQChar* uri;
        if (SQ_FAILED(sq_getstring(vm, 2, &uri))){
            return sq_throwerror(vm, "argument 1 \"uri\" is not of type string");
        }

        Lv2Plugin *obj;
        // call the implementation
        try {
            obj = Lv2PluginCache::instance().getPlugin(uri);
        }
        catch(std::exception const& e) {
            return sq_throwerror(vm, e.what());
        }

        // return pointer to new object
        sq_setinstanceup(vm, 1, (SQUserPointer*)obj);
        return 1;
    }
    SQObjectType overrideType = sq_gettype(vm, 3);
    SQUserPointer overrideTypeTag;
    if(overrideType == OT_INSTANCE) {
        sq_gettypetag(vm, 3, &overrideTypeTag);
    }

    if(overrideType == OT_STRING) {
        SQInteger numargs = sq_gettop(vm);
        // check parameter count
        if(numargs > 3) {
            return sq_throwerror(vm, "too many parameters, expected at most 2");
        }
        if(numargs < 2) {
            return sq_throwerror(vm, "insufficient parameters, expected at least 1");
        }
        // get parameter 1 "uri" as string
        const SQChar* uri;
        if (SQ_FAILED(sq_getstring(vm, 2, &uri))){
            return sq_throwerror(vm, "argument 1 \"uri\" is not of type string");
        }

        Lv2Plugin *obj;
        // 2 parameters passed in
        if(numargs == 3) {

            // get parameter 2 "preset" as string
            const SQChar* preset;
            if (SQ_FAILED(sq_getstring(vm, 3, &preset))){
                return sq_throwerror(vm, "argument 2 \"preset\" is not of type string");
            }

            // call the implementation
            try {
                obj = Lv2PluginCache::instance().getPlugin(uri, preset);
            }
            catch(std::exception const& e) {
                return sq_throwerror(vm, e.what());
            }
        }

        else {
            // call the implementation
            try {
                obj = Lv2PluginCache::instance().getPlugin(uri);
            }
            catch(std::exception const& e) {
                return sq_throwerror(vm, e.what());
            }
        }

        // return pointer to new object
        sq_setinstanceup(vm, 1, (SQUserPointer*)obj);
        return 1;
    }
    else if(overrideType == OT_INSTANCE && overrideTypeTag == &Lv2StateObject) {
        SQInteger numargs = sq_gettop(vm);
        // check parameter count
        if(numargs > 3) {
            return sq_throwerror(vm, "too many parameters, expected at most 2");
        }
        if(numargs < 2) {
            return sq_throwerror(vm, "insufficient parameters, expected at least 1");
        }
        // get parameter 1 "uri" as string
        const SQChar* uri;
        if (SQ_FAILED(sq_getstring(vm, 2, &uri))){
            return sq_throwerror(vm, "argument 1 \"uri\" is not of type string");
        }

        Lv2Plugin *obj;
        // 2 parameters passed in
        if(numargs == 3) {

            // get parameter 2 "state" as Lv2.State
            Lv2State *state = 0;
            sq_getinstanceup(vm, 3, (SQUserPointer*)&state, 0);
            if(state == 0) {
                return sq_throwerror(vm, "argument 2 \"state\" is not of type Lv2.State");
            }
            SQUserPointer stateTypeTag;
            sq_gettypetag(vm, 3, &stateTypeTag);
            if(stateTypeTag != &Lv2StateObject) {
                return sq_throwerror(vm, "argument 2 \"state\" is not of type Lv2.State");
            }

            // call the implementation
            try {
                obj = Lv2PluginCache::instance().getPlugin(uri, *state);
            }
            catch(std::exception const& e) {
                return sq_throwerror(vm, e.what());
            }
        }

        else {
            // call the implementation
            try {
                obj = Lv2PluginCache::instance().getPlugin(uri);
            }
            catch(std::exception const& e) {
                return sq_throwerror(vm, e.what());
            }
        }

        // return pointer to new object
        sq_setinstanceup(vm, 1, (SQUserPointer*)obj);
        return 1;
    }
    else {
        return sq_throwerror(vm, "argument 2 is not of type {string, Lv2.State}");
    }
}

//
// Lv2.Plugin addController
//
SQInteger Lv2PluginaddController(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 6) {
        return sq_throwerror(vm, "too many parameters, expected at most 5");
    }
    if(numargs < 4) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 3");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "addController method needs an instance of Plugin");
    }
    Lv2Plugin *obj = static_cast<Lv2Plugin*>(userPtr);

    // get parameter 1 "source" as Midi.Source
    MidiSource *source = getMidiSource(vm);
    if(source == 0) {
        return sq_throwerror(vm, "argument 1 \"source\" is not of type Midi.Source");
    }

    // get parameter 2 "cc" as integer
    SQInteger cc;
    if (SQ_FAILED(sq_getinteger(vm, 3, &cc))){
        return sq_throwerror(vm, "argument 2 \"cc\" is not of type integer");
    }

    // get parameter 3 "symbol" as string
    const SQChar* symbol;
    if (SQ_FAILED(sq_getstring(vm, 4, &symbol))){
        return sq_throwerror(vm, "argument 3 \"symbol\" is not of type string");
    }

    // 4 parameters passed in
    if(numargs == 5) {

        // get parameter 4 "mininum" as integer
        SQInteger mininum;
        if (SQ_FAILED(sq_getinteger(vm, 5, &mininum))){
            return sq_throwerror(vm, "argument 4 \"mininum\" is not of type integer");
        }

        // call the implementation
        try {
            obj->addController(*source, cc, symbol, mininum);
        }
        catch(std::exception const& e) {
            return sq_throwerror(vm, e.what());
        }
    }

    // 5 parameters passed in
    else if(numargs == 6) {

        // get parameter 4 "mininum" as integer
        SQInteger mininum;
        if (SQ_FAILED(sq_getinteger(vm, 5, &mininum))){
            return sq_throwerror(vm, "argument 4 \"mininum\" is not of type integer");
        }

        // get parameter 5 "maximum" as integer
        SQInteger maximum;
        if (SQ_FAILED(sq_getinteger(vm, 6, &maximum))){
            return sq_throwerror(vm, "argument 5 \"maximum\" is not of type integer");
        }

        // call the implementation
        try {
            obj->addController(*source, cc, symbol, mininum, maximum);
        }
        catch(std::exception const& e) {
            return sq_throwerror(vm, e.what());
        }
    }

    else {
        // call the implementation
        try {
            obj->addController(*source, cc, symbol);
        }
        catch(std::exception const& e) {
            return sq_throwerror(vm, e.what());
        }
    }

    // void method, returns no value
    return 0;
}

//
// Lv2.Plugin connect
//
SQInteger Lv2Pluginconnect(HSQUIRRELVM vm)
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
        return sq_throwerror(vm, "connect method needs an instance of Plugin");
    }
    Lv2Plugin *obj = static_cast<Lv2Plugin*>(userPtr);

    // get parameter 1 "source" as Audio.Source
    AudioSource *source = getAudioSource(vm);
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

//
// Lv2.Plugin connectMidi
//
SQInteger Lv2PluginconnectMidi(HSQUIRRELVM vm)
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
        return sq_throwerror(vm, "connectMidi method needs an instance of Plugin");
    }
    Lv2Plugin *obj = static_cast<Lv2Plugin*>(userPtr);

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
// Lv2.Plugin midiChannel
//
SQInteger Lv2PluginmidiChannel(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 2) {
        return sq_throwerror(vm, "too many parameters, expected at most 1");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "midiChannel method needs an instance of Plugin");
    }
    Lv2Plugin *obj = static_cast<Lv2Plugin*>(userPtr);

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
// Lv2.Plugin midiOutput
//
SQInteger Lv2PluginmidiOutput(HSQUIRRELVM vm)
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
        return sq_throwerror(vm, "midiOutput method needs an instance of Plugin");
    }
    Lv2Plugin *obj = static_cast<Lv2Plugin*>(userPtr);

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
// Lv2.Plugin onControl
//
SQInteger Lv2PluginonControl(HSQUIRRELVM vm)
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
        return sq_throwerror(vm, "onControl method needs an instance of Plugin");
    }
    Lv2Plugin *obj = static_cast<Lv2Plugin*>(userPtr);

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
// Lv2.Plugin output
//
SQInteger Lv2Pluginoutput(HSQUIRRELVM vm)
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
        return sq_throwerror(vm, "output method needs an instance of Plugin");
    }
    Lv2Plugin *obj = static_cast<Lv2Plugin*>(userPtr);

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
// Lv2.Plugin schedule
//
SQInteger Lv2Pluginschedule(HSQUIRRELVM vm)
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
            return sq_throwerror(vm, "schedule method needs an instance of Plugin");
        }
        Lv2Plugin *obj = static_cast<Lv2Plugin*>(userPtr);

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
            return sq_throwerror(vm, "schedule method needs an instance of Plugin");
        }
        Lv2Plugin *obj = static_cast<Lv2Plugin*>(userPtr);

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
            return sq_throwerror(vm, "schedule method needs an instance of Plugin");
        }
        Lv2Plugin *obj = static_cast<Lv2Plugin*>(userPtr);

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
// Lv2.Plugin scheduleControl
//
SQInteger Lv2PluginscheduleControl(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 6) {
        return sq_throwerror(vm, "too many parameters, expected at most 5");
    }
    if(numargs < 6) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 5");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "scheduleControl method needs an instance of Plugin");
    }
    Lv2Plugin *obj = static_cast<Lv2Plugin*>(userPtr);

    // get parameter 1 "control" as string
    const SQChar* control;
    if (SQ_FAILED(sq_getstring(vm, 2, &control))){
        return sq_throwerror(vm, "argument 1 \"control\" is not of type string");
    }

    // get parameter 2 "value" as float
    SQFloat value;
    if (SQ_FAILED(sq_getfloat(vm, 3, &value))){
        return sq_throwerror(vm, "argument 2 \"value\" is not of type float");
    }

    // get parameter 3 "bar" as integer
    SQInteger bar;
    if (SQ_FAILED(sq_getinteger(vm, 4, &bar))){
        return sq_throwerror(vm, "argument 3 \"bar\" is not of type integer");
    }

    // get parameter 4 "position" as integer
    SQInteger position;
    if (SQ_FAILED(sq_getinteger(vm, 5, &position))){
        return sq_throwerror(vm, "argument 4 \"position\" is not of type integer");
    }

    // get parameter 5 "division" as integer
    SQInteger division;
    if (SQ_FAILED(sq_getinteger(vm, 6, &division))){
        return sq_throwerror(vm, "argument 5 \"division\" is not of type integer");
    }

    // call the implementation
    try {
        obj->scheduleControl(control, value, bar, position, division);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // void method, returns no value
    return 0;
}

//
// Lv2.Plugin setControl
//
SQInteger Lv2PluginsetControl(HSQUIRRELVM vm)
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
        return sq_throwerror(vm, "setControl method needs an instance of Plugin");
    }
    Lv2Plugin *obj = static_cast<Lv2Plugin*>(userPtr);

    // get parameter 1 "control" as string
    const SQChar* control;
    if (SQ_FAILED(sq_getstring(vm, 2, &control))){
        return sq_throwerror(vm, "argument 1 \"control\" is not of type string");
    }

    // get parameter 2 "value" as float
    SQFloat value;
    if (SQ_FAILED(sq_getfloat(vm, 3, &value))){
        return sq_throwerror(vm, "argument 2 \"value\" is not of type float");
    }

    // call the implementation
    try {
        obj->setControlValue(control, value);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // void method, returns no value
    return 0;
}

//
// Lv2.State class
//
SQInteger Lv2StateRelease(SQUserPointer p, SQInteger size)
{
    delete static_cast<Lv2State*>(p);
}

SQInteger Lv2StateCtor(HSQUIRRELVM vm)
{
    SQObjectType overrideType = sq_gettype(vm, 2);
    SQUserPointer overrideTypeTag;
    if(overrideType == OT_INSTANCE) {
        sq_gettypetag(vm, 2, &overrideTypeTag);
    }

    if(overrideType == OT_TABLE) {
        SQInteger numargs = sq_gettop(vm);
        // check parameter count
        if(numargs > 2) {
            return sq_throwerror(vm, "too many parameters, expected at most 1");
        }
        if(numargs < 2) {
            return sq_throwerror(vm, "insufficient parameters, expected at least 1");
        }
        // get parameter 1 "state" as table
        HSQOBJECT stateObj;
        if (SQ_FAILED(sq_getstackobj(vm, 2, &stateObj))) {
            return sq_throwerror(vm, "argument 1 \"state\" is not of type table");
        }
        if (sq_gettype(vm, 2) != OT_TABLE) {
            return sq_throwerror(vm, "argument 1 \"state\" is not of type table");
        }
        ScriptHashIterator state(vm, stateObj);

        Lv2State *obj;
        // call the implementation
        try {
            obj = new Lv2State(state);
        }
        catch(std::exception const& e) {
            return sq_throwerror(vm, e.what());
        }

        // return pointer to new object
        sq_setinstanceup(vm, 1, (SQUserPointer*)obj);
        sq_setreleasehook(vm, 1, Lv2StateRelease);
        return 1;
    }
    else if(overrideType == OT_STRING) {
        SQInteger numargs = sq_gettop(vm);
        // check parameter count
        if(numargs > 2) {
            return sq_throwerror(vm, "too many parameters, expected at most 1");
        }
        if(numargs < 2) {
            return sq_throwerror(vm, "insufficient parameters, expected at least 1");
        }
        // get parameter 1 "file" as string
        const SQChar* file;
        if (SQ_FAILED(sq_getstring(vm, 2, &file))){
            return sq_throwerror(vm, "argument 1 \"file\" is not of type string");
        }

        Lv2State *obj;
        // call the implementation
        try {
            obj = new Lv2State(file);
        }
        catch(std::exception const& e) {
            return sq_throwerror(vm, e.what());
        }

        // return pointer to new object
        sq_setinstanceup(vm, 1, (SQUserPointer*)obj);
        sq_setreleasehook(vm, 1, Lv2StateRelease);
        return 1;
    }
    else {
        return sq_throwerror(vm, "argument 1 is not of type {table, string}");
    }
}


void bindLv2(HSQUIRRELVM vm)
{
    // create package table
    sq_pushstring(vm, "Lv2", -1);
    sq_newtable(vm);

    // create class Lv2.Plugin
    sq_pushstring(vm, "Plugin", -1);
    sq_newclass(vm, false);
    sq_getstackobj(vm, -1, &Lv2PluginObject);
    sq_settypetag(vm, -1, &Lv2PluginObject);

    // ctor for class Plugin
    sq_pushstring(vm, _SC("constructor"), -1);
    sq_newclosure(vm, &Lv2PluginCtor, 0);
    sq_newslot(vm, -3, false);

    // methods for class Plugin
    sq_pushstring(vm, _SC("addController"), -1);
    sq_newclosure(vm, &Lv2PluginaddController, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("connect"), -1);
    sq_newclosure(vm, &Lv2Pluginconnect, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("connectMidi"), -1);
    sq_newclosure(vm, &Lv2PluginconnectMidi, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("midiChannel"), -1);
    sq_newclosure(vm, &Lv2PluginmidiChannel, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("midiOutput"), -1);
    sq_newclosure(vm, &Lv2PluginmidiOutput, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("onControl"), -1);
    sq_newclosure(vm, &Lv2PluginonControl, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("output"), -1);
    sq_newclosure(vm, &Lv2Pluginoutput, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("schedule"), -1);
    sq_newclosure(vm, &Lv2Pluginschedule, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("scheduleControl"), -1);
    sq_newclosure(vm, &Lv2PluginscheduleControl, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("setControl"), -1);
    sq_newclosure(vm, &Lv2PluginsetControl, 0);
    sq_newslot(vm, -3, false);

    // push Plugin to Lv2 package table
    sq_newslot(vm, -3, false);

    // create class Lv2.State
    sq_pushstring(vm, "State", -1);
    sq_newclass(vm, false);
    sq_getstackobj(vm, -1, &Lv2StateObject);
    sq_settypetag(vm, -1, &Lv2StateObject);

    // ctor for class State
    sq_pushstring(vm, _SC("constructor"), -1);
    sq_newclosure(vm, &Lv2StateCtor, 0);
    sq_newslot(vm, -3, false);

    // methods for class State
    // push State to Lv2 package table
    sq_newslot(vm, -3, false);

    // push package "Lv2" to root table
    sq_newslot(vm, -3, false);
}
}