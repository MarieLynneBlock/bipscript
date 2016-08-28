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

#include "bindtransport.h"
#include "bindtypes.h"
#include "bindings.h"

#include "transport.h"
#include "audioengine.h"
#include "transportmaster.h"
#include "timesignature.h"

namespace binding {

// object references to types in this package
HSQOBJECT TransportMasterObject;
HSQOBJECT TransportTimeSignatureObject;
//
// Transport schedule
//
SQInteger Transportschedule(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs < 3) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 2");
    }
    // get parameter 1 "function" as function
    HSQOBJECT functionObj;
    if (SQ_FAILED(sq_getstackobj(vm, 2, &functionObj))) {
        return sq_throwerror(vm, "argument 1 is not of type function");
    }
    if (sq_gettype(vm, 2) != OT_CLOSURE) {
        return sq_throwerror(vm, "argument 1 is not of type function");
    }
    SQUnsignedInteger nparams, nfreevars;
    sq_getclosureinfo(vm, 2, &nparams, &nfreevars);
    sq_addref(vm, &functionObj);
    ScriptFunction function(vm, functionObj, nparams);

    // get parameter 2 "bar" as integer
    SQInteger bar;
    if (SQ_FAILED(sq_getinteger(vm, 3, &bar))){
        return sq_throwerror(vm, "argument 2 is not of type integer");
    }

    // 3 parameters passed in
    if(numargs == 4) {

        // get parameter 3 "position" as integer
        SQInteger position;
        if (SQ_FAILED(sq_getinteger(vm, 4, &position))){
            return sq_throwerror(vm, "argument 3 is not of type integer");
        }

        // call the implementation
        try {
            Transport::instance().schedule(function, bar, position);
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
            return sq_throwerror(vm, "argument 3 is not of type integer");
        }

        // get parameter 4 "division" as integer
        SQInteger division;
        if (SQ_FAILED(sq_getinteger(vm, 5, &division))){
            return sq_throwerror(vm, "argument 4 is not of type integer");
        }

        // call the implementation
        try {
            Transport::instance().schedule(function, bar, position, division);
        }
        catch(std::exception const& e) {
            return sq_throwerror(vm, e.what());
        }
    }

    else {
        // call the implementation
        try {
            Transport::instance().schedule(function, bar);
        }
        catch(std::exception const& e) {
            return sq_throwerror(vm, e.what());
        }
    }

    // void method, returns no value
    return 0;
}
//
// Transport start
//
SQInteger Transportstart(HSQUIRRELVM vm)
{
    // call the implementation
    try {
        AudioEngine::instance().transportStart();
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // void method, returns no value
    return 0;
}
//
// Transport stop
//
SQInteger Transportstop(HSQUIRRELVM vm)
{
    // call the implementation
    try {
        AudioEngine::instance().transportStop();
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // void method, returns no value
    return 0;
}

//
// Transport.Master class
//
SQInteger TransportMasterCtor(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs < 2) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 1");
    }
    // get parameter 1 "bpm" as float
    SQFloat bpm;
    if (SQ_FAILED(sq_getfloat(vm, 2, &bpm))){
        return sq_throwerror(vm, "argument 1 is not of type float");
    }

    TransportMaster *obj;
    // 2 parameters passed in
    if(numargs == 3) {

        // get parameter 2 "numerator" as integer
        SQInteger numerator;
        if (SQ_FAILED(sq_getinteger(vm, 3, &numerator))){
            return sq_throwerror(vm, "argument 2 is not of type integer");
        }

        // call the implementation
        try {
            obj = TransportMasterCache::instance().getTransportMaster(bpm, numerator);
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
            return sq_throwerror(vm, "argument 2 is not of type integer");
        }

        // get parameter 3 "denominator" as integer
        SQInteger denominator;
        if (SQ_FAILED(sq_getinteger(vm, 4, &denominator))){
            return sq_throwerror(vm, "argument 3 is not of type integer");
        }

        // call the implementation
        try {
            obj = TransportMasterCache::instance().getTransportMaster(bpm, numerator, denominator);
        }
        catch(std::exception const& e) {
            return sq_throwerror(vm, e.what());
        }
    }

    else {
        // call the implementation
        try {
            obj = TransportMasterCache::instance().getTransportMaster(bpm);
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
// Transport.Master setTimeSignature
//
SQInteger TransportMastersetTimeSignature(HSQUIRRELVM vm)
{
    SQObjectType overrideType = sq_gettype(vm, 2);
    SQUserPointer overrideTypeTag;
    if(overrideType == OT_INSTANCE) {
        sq_gettypetag(vm, 2, &overrideTypeTag);
    }

    if(overrideType == OT_INSTANCE && overrideTypeTag == &TransportTimeSignatureObject) {
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs < 2) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 1");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "setTimeSignature method needs an instance of Master");
    }
    TransportMaster *obj = static_cast<TransportMaster*>(userPtr);

    // get parameter 1 "signature" as Transport.TimeSignature
    TimeSignature *signature = 0;
    sq_getinstanceup(vm, 2, (SQUserPointer*)&signature, 0);
    if(signature == 0) {
        return sq_throwerror(vm, "argument 1 is not of type Transport.TimeSignature");
    }

    // call the implementation
    try {
        obj->setTimeSignature(*signature);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // void method, returns no value
    return 0;
    }
    if(overrideType == OT_INTEGER) {
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs < 3) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 2");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "setTimeSignature method needs an instance of Master");
    }
    TransportMaster *obj = static_cast<TransportMaster*>(userPtr);

    // get parameter 1 "numerator" as integer
    SQInteger numerator;
    if (SQ_FAILED(sq_getinteger(vm, 2, &numerator))){
        return sq_throwerror(vm, "argument 1 is not of type integer");
    }

    // get parameter 2 "denominator" as integer
    SQInteger denominator;
    if (SQ_FAILED(sq_getinteger(vm, 3, &denominator))){
        return sq_throwerror(vm, "argument 2 is not of type integer");
    }

    // call the implementation
    try {
        obj->setTimeSignature(numerator, denominator);
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
// Transport.TimeSignature class
//

//
// Transport.TimeSignature numerator
//
SQInteger TransportTimeSignaturenumerator(HSQUIRRELVM vm)
{
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "numerator method needs an instance of TimeSignature");
    }
    TimeSignature *obj = static_cast<TimeSignature*>(userPtr);

    // return value
    SQInteger ret;
    // call the implementation
    try {
        ret = obj->getNumerator();
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // push return value
    sq_pushinteger(vm, ret);
    return 1;
}

//
// Transport.TimeSignature denominator
//
SQInteger TransportTimeSignaturedenominator(HSQUIRRELVM vm)
{
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "denominator method needs an instance of TimeSignature");
    }
    TimeSignature *obj = static_cast<TimeSignature*>(userPtr);

    // return value
    SQInteger ret;
    // call the implementation
    try {
        ret = obj->getDenominator();
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // push return value
    sq_pushinteger(vm, ret);
    return 1;
}


void bindTransport(HSQUIRRELVM vm)
{
    // create package table
    sq_pushstring(vm, "Transport", -1);
    sq_newtable(vm);

    // static method schedule
    sq_pushstring(vm, _SC("schedule"), -1);
    sq_newclosure(vm, &Transportschedule, 0);
    sq_newslot(vm, -3, false);

    // static method start
    sq_pushstring(vm, _SC("start"), -1);
    sq_newclosure(vm, &Transportstart, 0);
    sq_newslot(vm, -3, false);

    // static method stop
    sq_pushstring(vm, _SC("stop"), -1);
    sq_newclosure(vm, &Transportstop, 0);
    sq_newslot(vm, -3, false);

    // create class Transport.Master
    sq_pushstring(vm, "Master", -1);
    sq_newclass(vm, false);
    sq_getstackobj(vm, -1, &TransportMasterObject);
    sq_settypetag(vm, -1, &TransportMasterObject);

    // ctor for class Master
    sq_pushstring(vm, _SC("constructor"), -1);
    sq_newclosure(vm, &TransportMasterCtor, 0);
    sq_newslot(vm, -3, false);

    // methods for class Master
    sq_pushstring(vm, _SC("setTimeSignature"), -1);
    sq_newclosure(vm, &TransportMastersetTimeSignature, 0);
    sq_newslot(vm, -3, false);

    // push Master to Transport package table
    sq_newslot(vm, -3, false);

    // create class Transport.TimeSignature
    sq_pushstring(vm, "TimeSignature", -1);
    sq_newclass(vm, false);
    sq_getstackobj(vm, -1, &TransportTimeSignatureObject);
    sq_settypetag(vm, -1, &TransportTimeSignatureObject);


    // methods for class TimeSignature
    sq_pushstring(vm, _SC("numerator"), -1);
    sq_newclosure(vm, &TransportTimeSignaturenumerator, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("denominator"), -1);
    sq_newclosure(vm, &TransportTimeSignaturedenominator, 0);
    sq_newslot(vm, -3, false);

    // push TimeSignature to Transport package table
    sq_newslot(vm, -3, false);

    // push package "Transport" to root table
    sq_newslot(vm, -3, false);
}
}