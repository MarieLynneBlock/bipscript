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

#include "bindmath.h"
#include "bindtypes.h"
#include "bindings.h"

#include "random.h"

namespace binding {

// object references to types in this package
HSQOBJECT MathRandomObject;

//
// Math.Random class
//
SQInteger MathRandomCtor(HSQUIRRELVM vm)
{
    Random *obj;
    // call the implementation
    try {
        obj = new Random();
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
// Math.Random integer
//
SQInteger MathRandominteger(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs < 2) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 1");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    sq_getinstanceup(vm, 1, &userPtr, 0);
    Random *obj = static_cast<Random*>(userPtr);

    // get parameter 1 "max" as integer
    SQInteger max;
    if (SQ_FAILED(sq_getinteger(vm, 2, &max))){
        return sq_throwerror(vm, "argument 1 is not of type integer");
    }

    // return value
    SQInteger ret;
    // call the implementation
    try {
        ret = obj->integer(max);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // push return value
    sq_pushinteger(vm, ret);
    return 1;
}


void bindMath(HSQUIRRELVM vm)
{
    // create package table
    sq_pushstring(vm, "Math", -1);
    sq_newtable(vm);

    // create class Math.Random
    sq_pushstring(vm, "Random", -1);
    sq_newclass(vm, false);
    sq_getstackobj(vm, -1, &MathRandomObject);
    sq_settypetag(vm, -1, &MathRandomObject);

    // ctor for class Random
    sq_pushstring(vm, _SC("constructor"), -1);
    sq_newclosure(vm, &MathRandomCtor, 0);
    sq_newslot(vm, -3, false);

    // methods for class Random
    sq_pushstring(vm, _SC("integer"), -1);
    sq_newclosure(vm, &MathRandominteger, 0);
    sq_newslot(vm, -3, false);

    // push Random to Math package table
    sq_newslot(vm, -3, false);

    // push package "Math" to root table
    sq_newslot(vm, -3, false);
}
}