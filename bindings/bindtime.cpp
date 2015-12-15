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

#include "bindtime.h"
#include "bindtypes.h"
#include "bindings.h"

#include "timepackage.h"
#include "audioengine.h"
#include "timesignature.h"
#include "transportmaster.h"

namespace binding {

// object references to types in this package
HSQOBJECT TimePositionObject;
HSQOBJECT TimeSignatureObject;
HSQOBJECT TimeTransportMasterObject;
//
// Time schedule
//
SQInteger Timeschedule(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs < 3) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 2");
    }
    // get parameter 1 "function" as function
    HSQOBJECT function;
    sq_getstackobj(vm, 2, &function);
    sq_addref(vm, &function);

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
            Time::schedule(function, bar, position);
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
            Time::schedule(function, bar, position, division);
        }
        catch(std::exception const& e) {
            return sq_throwerror(vm, e.what());
        }
    }

    else {
        // call the implementation
        try {
            Time::schedule(function, bar);
        }
        catch(std::exception const& e) {
            return sq_throwerror(vm, e.what());
        }
    }

    // void method, returns no value
    return 0;
}
//
// Time start
//
SQInteger Timestart(HSQUIRRELVM vm)
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
// Time stop
//
SQInteger Timestop(HSQUIRRELVM vm)
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
// Time.Position class
//

//
// Time.Signature class
//

//
// Time.Signature numerator
//
SQInteger TimeSignaturenumerator(HSQUIRRELVM vm)
{
    // get "this" pointer
    SQUserPointer userPtr = 0;
    sq_getinstanceup(vm, 1, &userPtr, 0);
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
// Time.Signature denominator
//
SQInteger TimeSignaturedenominator(HSQUIRRELVM vm)
{
    // get "this" pointer
    SQUserPointer userPtr = 0;
    sq_getinstanceup(vm, 1, &userPtr, 0);
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

//
// Time.TransportMaster class
//
SQInteger TimeTransportMasterCtor(HSQUIRRELVM vm)
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
    // call the implementation
    try {
        obj = TransportMasterCache::instance().getTransportMaster(bpm);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // return pointer to new object
    sq_setinstanceup(vm, 1, (SQUserPointer*)obj);
    //sq_setreleasehook(vm, 1, release_hook);
    return 1;
}


void bindTime(HSQUIRRELVM vm)
{
    // create package table
    sq_pushstring(vm, "Time", -1);
    sq_newtable(vm);

    // static method schedule
    sq_pushstring(vm, _SC("schedule"), -1);
    sq_newclosure(vm, &Timeschedule, 0);
    sq_newslot(vm, -3, false);

    // static method start
    sq_pushstring(vm, _SC("start"), -1);
    sq_newclosure(vm, &Timestart, 0);
    sq_newslot(vm, -3, false);

    // static method stop
    sq_pushstring(vm, _SC("stop"), -1);
    sq_newclosure(vm, &Timestop, 0);
    sq_newslot(vm, -3, false);

    // create class Time.Position
    sq_pushstring(vm, "Position", -1);
    sq_newclass(vm, false);
    sq_getstackobj(vm, -1, &TimePositionObject);
    sq_settypetag(vm, -1, &TimePositionObject);


    // methods for class Position
    // push Position to Time package table
    sq_newslot(vm, -3, false);

    // create class Time.Signature
    sq_pushstring(vm, "Signature", -1);
    sq_newclass(vm, false);
    sq_getstackobj(vm, -1, &TimeSignatureObject);
    sq_settypetag(vm, -1, &TimeSignatureObject);


    // methods for class Signature
    sq_pushstring(vm, _SC("numerator"), -1);
    sq_newclosure(vm, &TimeSignaturenumerator, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("denominator"), -1);
    sq_newclosure(vm, &TimeSignaturedenominator, 0);
    sq_newslot(vm, -3, false);

    // push Signature to Time package table
    sq_newslot(vm, -3, false);

    // create class Time.TransportMaster
    sq_pushstring(vm, "TransportMaster", -1);
    sq_newclass(vm, false);
    sq_getstackobj(vm, -1, &TimeTransportMasterObject);
    sq_settypetag(vm, -1, &TimeTransportMasterObject);

    // ctor for class TransportMaster
    sq_pushstring(vm, _SC("constructor"), -1);
    sq_newclosure(vm, &TimeTransportMasterCtor, 0);
    sq_newslot(vm, -3, false);

    // methods for class TransportMaster
    // push TransportMaster to Time package table
    sq_newslot(vm, -3, false);

    // push package "Time" to root table
    sq_newslot(vm, -3, false);
}
}