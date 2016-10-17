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

#include "bindosc.h"
#include "bindtypes.h"

#include "oscinput.h"
#include "oscmessage.h"
#include "oscoutput.h"

namespace binding {

// object references to types in this package
HSQOBJECT OscInputObject;
HSQOBJECT OscMessageObject;
HSQOBJECT OscOutputObject;

//
// Osc.Input class
//
SQInteger OscInputCtor(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 3) {
        return sq_throwerror(vm, "too many parameters, expected at most 2");
    }
    if(numargs < 2) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 1");
    }
    // get parameter 1 "port" as integer
    SQInteger port;
    if (SQ_FAILED(sq_getinteger(vm, 2, &port))){
        return sq_throwerror(vm, "argument 1 \"port\" is not of type integer");
    }

    OscInput *obj;
    // 2 parameters passed in
    if(numargs == 3) {

        // get parameter 2 "protocol" as string
        const SQChar* protocol;
        if (SQ_FAILED(sq_getstring(vm, 3, &protocol))){
            return sq_throwerror(vm, "argument 2 \"protocol\" is not of type string");
        }

        // call the implementation
        try {
            obj = OscInputFactory::instance().getOscInput(port, protocol);
        }
        catch(std::exception const& e) {
            return sq_throwerror(vm, e.what());
        }
    }

    else {
        // call the implementation
        try {
            obj = OscInputFactory::instance().getOscInput(port);
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
// Osc.Input onReceive
//
SQInteger OscInputonReceive(HSQUIRRELVM vm)
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
        return sq_throwerror(vm, "onReceive method needs an instance of Input");
    }
    OscInput *obj = static_cast<OscInput*>(userPtr);

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
        obj->onReceive(handler);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // void method, returns no value
    return 0;
}

//
// Osc.Input url
//
SQInteger OscInputurl(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 1) {
        return sq_throwerror(vm, "too many parameters, expected at most 0");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "url method needs an instance of Input");
    }
    OscInput *obj = static_cast<OscInput*>(userPtr);

    // return value
    const SQChar* ret;
    // call the implementation
    try {
        ret = obj->getUrl();
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // push return value
    sq_pushstring(vm, ret, strlen(ret));
    return 1;
}

//
// Osc.Message class
//
SQInteger OscMessageRelease(SQUserPointer p, SQInteger size)
{
    delete static_cast<OscMessage*>(p);
}

SQInteger OscMessageCtor(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 2) {
        return sq_throwerror(vm, "too many parameters, expected at most 1");
    }
    if(numargs < 2) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 1");
    }
    // get parameter 1 "path" as string
    const SQChar* path;
    if (SQ_FAILED(sq_getstring(vm, 2, &path))){
        return sq_throwerror(vm, "argument 1 \"path\" is not of type string");
    }

    OscMessage *obj;
    // call the implementation
    try {
        obj = new OscMessage(path);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // return pointer to new object
    sq_setinstanceup(vm, 1, (SQUserPointer*)obj);
    sq_setreleasehook(vm, 1, OscMessageRelease);
    return 1;
}

//
// Osc.Message add
//
SQInteger OscMessageadd(HSQUIRRELVM vm)
{
    SQObjectType overrideType = sq_gettype(vm, 2);
    SQUserPointer overrideTypeTag;
    if(overrideType == OT_INSTANCE) {
        sq_gettypetag(vm, 2, &overrideTypeTag);
    }

    if(overrideType == OT_INTEGER) {
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
            return sq_throwerror(vm, "add method needs an instance of Message");
        }
        OscMessage *obj = static_cast<OscMessage*>(userPtr);

        // get parameter 1 "value" as integer
        SQInteger value;
        if (SQ_FAILED(sq_getinteger(vm, 2, &value))){
            return sq_throwerror(vm, "argument 1 \"value\" is not of type integer");
        }

        // call the implementation
        try {
            obj->addInteger(value);
        }
        catch(std::exception const& e) {
            return sq_throwerror(vm, e.what());
        }

        // void method, returns no value
        return 0;
    }
    else if(overrideType == OT_FLOAT) {
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
            return sq_throwerror(vm, "add method needs an instance of Message");
        }
        OscMessage *obj = static_cast<OscMessage*>(userPtr);

        // get parameter 1 "value" as float
        SQFloat value;
        if (SQ_FAILED(sq_getfloat(vm, 2, &value))){
            return sq_throwerror(vm, "argument 1 \"value\" is not of type float");
        }

        // call the implementation
        try {
            obj->addFloat(value);
        }
        catch(std::exception const& e) {
            return sq_throwerror(vm, e.what());
        }

        // void method, returns no value
        return 0;
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
        // get "this" pointer
        SQUserPointer userPtr = 0;
        if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
            return sq_throwerror(vm, "add method needs an instance of Message");
        }
        OscMessage *obj = static_cast<OscMessage*>(userPtr);

        // get parameter 1 "value" as string
        const SQChar* value;
        if (SQ_FAILED(sq_getstring(vm, 2, &value))){
            return sq_throwerror(vm, "argument 1 \"value\" is not of type string");
        }

        // call the implementation
        try {
            obj->addString(value);
        }
        catch(std::exception const& e) {
            return sq_throwerror(vm, e.what());
        }

        // void method, returns no value
        return 0;
    }
    else if(overrideType == OT_BOOL) {
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
            return sq_throwerror(vm, "add method needs an instance of Message");
        }
        OscMessage *obj = static_cast<OscMessage*>(userPtr);

        // get parameter 1 "value" as bool
        SQBool value;
        if (SQ_FAILED(sq_getbool(vm, 2, &value))){
            return sq_throwerror(vm, "argument 1 \"value\" is not of type bool");
        }

        // call the implementation
        try {
            obj->addBoolean(value);
        }
        catch(std::exception const& e) {
            return sq_throwerror(vm, e.what());
        }

        // void method, returns no value
        return 0;
    }
    else if(overrideType == OT_NULL) {
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
            return sq_throwerror(vm, "add method needs an instance of Message");
        }
        OscMessage *obj = static_cast<OscMessage*>(userPtr);

        // get parameter 1 "value" as null
        const char *value = 0;

        // call the implementation
        try {
            obj->addNull(value);
        }
        catch(std::exception const& e) {
            return sq_throwerror(vm, e.what());
        }

        // void method, returns no value
        return 0;
    }
    else {
        return sq_throwerror(vm, "argument 1 is not of type {integer, float, string, bool, null}");
    }
}

//
// Osc.Message path
//
SQInteger OscMessagepath(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 1) {
        return sq_throwerror(vm, "too many parameters, expected at most 0");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "path method needs an instance of Message");
    }
    OscMessage *obj = static_cast<OscMessage*>(userPtr);

    // return value
    const SQChar* ret;
    // call the implementation
    try {
        ret = obj->getPath();
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // push return value
    sq_pushstring(vm, ret, strlen(ret));
    return 1;
}

//
// Osc.Output class
//
SQInteger OscOutputCtor(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 3) {
        return sq_throwerror(vm, "too many parameters, expected at most 2");
    }
    if(numargs < 3) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 2");
    }
    // get parameter 1 "host" as string
    const SQChar* host;
    if (SQ_FAILED(sq_getstring(vm, 2, &host))){
        return sq_throwerror(vm, "argument 1 \"host\" is not of type string");
    }

    // get parameter 2 "port" as integer
    SQInteger port;
    if (SQ_FAILED(sq_getinteger(vm, 3, &port))){
        return sq_throwerror(vm, "argument 2 \"port\" is not of type integer");
    }

    OscOutput *obj;
    // call the implementation
    try {
        obj = OscOutputFactory::instance().getOscOutput(host, port);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // return pointer to new object
    sq_setinstanceup(vm, 1, (SQUserPointer*)obj);
    return 1;
}

//
// Osc.Output schedule
//
SQInteger OscOutputschedule(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 5) {
        return sq_throwerror(vm, "too many parameters, expected at most 4");
    }
    if(numargs < 3) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 2");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "schedule method needs an instance of Output");
    }
    OscOutput *obj = static_cast<OscOutput*>(userPtr);

    // get parameter 1 "message" as Osc.Message
    OscMessage *message = 0;
    sq_getinstanceup(vm, 2, (SQUserPointer*)&message, 0);
    if(message == 0) {
        return sq_throwerror(vm, "argument 1 \"message\" is not of type Osc.Message");
    }
    SQUserPointer messageTypeTag;
    sq_gettypetag(vm, 2, &messageTypeTag);
    if(messageTypeTag != &OscMessageObject) {
        return sq_throwerror(vm, "argument 1 \"message\" is not of type Osc.Message");
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


void bindOsc(HSQUIRRELVM vm)
{
    // create package table
    sq_pushstring(vm, "Osc", -1);
    sq_newtable(vm);

    // create class Osc.Input
    sq_pushstring(vm, "Input", -1);
    sq_newclass(vm, false);
    sq_getstackobj(vm, -1, &OscInputObject);
    sq_settypetag(vm, -1, &OscInputObject);

    // ctor for class Input
    sq_pushstring(vm, _SC("constructor"), -1);
    sq_newclosure(vm, &OscInputCtor, 0);
    sq_newslot(vm, -3, false);

    // methods for class Input
    sq_pushstring(vm, _SC("onReceive"), -1);
    sq_newclosure(vm, &OscInputonReceive, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("url"), -1);
    sq_newclosure(vm, &OscInputurl, 0);
    sq_newslot(vm, -3, false);

    // push Input to Osc package table
    sq_newslot(vm, -3, false);

    // create class Osc.Message
    sq_pushstring(vm, "Message", -1);
    sq_newclass(vm, false);
    sq_getstackobj(vm, -1, &OscMessageObject);
    sq_settypetag(vm, -1, &OscMessageObject);

    // ctor for class Message
    sq_pushstring(vm, _SC("constructor"), -1);
    sq_newclosure(vm, &OscMessageCtor, 0);
    sq_newslot(vm, -3, false);

    // methods for class Message
    sq_pushstring(vm, _SC("add"), -1);
    sq_newclosure(vm, &OscMessageadd, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("path"), -1);
    sq_newclosure(vm, &OscMessagepath, 0);
    sq_newslot(vm, -3, false);

    // push Message to Osc package table
    sq_newslot(vm, -3, false);

    // create class Osc.Output
    sq_pushstring(vm, "Output", -1);
    sq_newclass(vm, false);
    sq_getstackobj(vm, -1, &OscOutputObject);
    sq_settypetag(vm, -1, &OscOutputObject);

    // ctor for class Output
    sq_pushstring(vm, _SC("constructor"), -1);
    sq_newclosure(vm, &OscOutputCtor, 0);
    sq_newslot(vm, -3, false);

    // methods for class Output
    sq_pushstring(vm, _SC("schedule"), -1);
    sq_newclosure(vm, &OscOutputschedule, 0);
    sq_newslot(vm, -3, false);

    // push Output to Osc package table
    sq_newslot(vm, -3, false);

    // push package "Osc" to root table
    sq_newslot(vm, -3, false);
}
}