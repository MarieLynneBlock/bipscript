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

#include "bindexp.h"
#include "bindtypes.h"
#include "bindings.h"

#include "midiinputbuffer.h"

namespace binding {

// object references to types in this package
HSQOBJECT ExpInputBufferObject;

//
// Exp.InputBuffer class
//
SQInteger ExpInputBufferCtor(HSQUIRRELVM vm)
{
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
// Exp.InputBuffer getLastControl
//
SQInteger ExpInputBuffergetLastControl(HSQUIRRELVM vm)
{
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
        ret = obj->getLastControl(control);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // push return value
    sq_pushinteger(vm, ret);
    return 1;
}


void bindExp(HSQUIRRELVM vm)
{
    // create package table
    sq_pushstring(vm, "Exp", -1);
    sq_newtable(vm);

    // create class Exp.InputBuffer
    sq_pushstring(vm, "InputBuffer", -1);
    sq_newclass(vm, false);
    sq_getstackobj(vm, -1, &ExpInputBufferObject);
    sq_settypetag(vm, -1, &ExpInputBufferObject);

    // ctor for class InputBuffer
    sq_pushstring(vm, _SC("constructor"), -1);
    sq_newclosure(vm, &ExpInputBufferCtor, 0);
    sq_newslot(vm, -3, false);

    // methods for class InputBuffer
    sq_pushstring(vm, _SC("getLastControl"), -1);
    sq_newclosure(vm, &ExpInputBuffergetLastControl, 0);
    sq_newslot(vm, -3, false);

    // push InputBuffer to Exp package table
    sq_newslot(vm, -3, false);

    // push package "Exp" to root table
    sq_newslot(vm, -3, false);
}
}