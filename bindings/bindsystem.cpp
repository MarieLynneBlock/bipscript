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

#include "bindsystem.h"
#include "bindtypes.h"

#include "systempackage.h"

namespace binding {

// object references to types in this package

//
// System argument
//
SQInteger Systemargument(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 2) {
        return sq_throwerror(vm, "too many parameters, expected at most 1");
    }
    if(numargs < 2) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 1");
    }
    // get parameter 1 "index" as integer
    SQInteger index;
    if (SQ_FAILED(sq_getinteger(vm, 2, &index))){
        return sq_throwerror(vm, "argument 1 is not of type integer");
    }

    // return value
    const SQChar* ret;
    // call the implementation
    try {
        ret = System::getArgument(index);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // push return value
    if(!ret) {
    sq_pushnull(vm);
    } else {
    sq_pushstring(vm, ret, strlen(ret));
    }
    return 1;
}


void bindSystem(HSQUIRRELVM vm)
{
    // create package table
    sq_pushstring(vm, "System", -1);
    sq_newtable(vm);

    // static method argument
    sq_pushstring(vm, _SC("argument"), -1);
    sq_newclosure(vm, &Systemargument, 0);
    sq_newslot(vm, -3, false);

    // push package "System" to root table
    sq_newslot(vm, -3, false);
}
}