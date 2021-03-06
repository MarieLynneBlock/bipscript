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

#include "bindio.h"
#include "bindtypes.h"
#include "bindcommon.h"

#include "io.h"
#include <stdexcept>
#include <cstring>

namespace bipscript {

using namespace io;

namespace binding {

// object references to types in this package
HSQOBJECT IOFileObject;

//
// IO.File class
//
SQInteger IOFileRelease(SQUserPointer p, SQInteger size)
{
    delete static_cast<File*>(p);
}

SQInteger IOFileCtor(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 2) {
        return sq_throwerror(vm, "too many parameters, expected at most 1");
    }
    if(numargs < 2) {
        return sq_throwerror(vm, "insufficient parameters, expected at least 1");
    }
    // get parameter 1 "name" as string
    const SQChar* name;
    if (SQ_FAILED(sq_getstring(vm, 2, &name))){
        return sq_throwerror(vm, "argument 1 \"name\" is not of type string");
    }

    File *obj;
    // call the implementation
    try {
        obj = new File(name);
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // return pointer to new object
    sq_setinstanceup(vm, 1, (SQUserPointer*)obj);
    sq_setreleasehook(vm, 1, IOFileRelease);
    return 1;
}

SQInteger IOFileClone(HSQUIRRELVM vm)
{
    // get instance ptr of original
    SQUserPointer userPtr;
    sq_getinstanceup(vm, 2, &userPtr, 0);
    // set instance ptr to a copy
    sq_setinstanceup(vm, 1, new File(*(File*)userPtr));
    sq_setreleasehook(vm, 1, &IOFileRelease);
    return 0;
}

//
// IO.File exists
//
SQInteger IOFileexists(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 1) {
        return sq_throwerror(vm, "too many parameters, expected at most 0");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "exists method needs an instance of File");
    }
    File *obj = static_cast<File*>(userPtr);
    if(!obj) {
        return sq_throwerror(vm, "exists method called before IO.File constructor");
    }
    // return value
    SQBool ret;
    // call the implementation
    try {
        ret = obj->exists();
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // push return value
    sq_pushbool(vm, ret);
    return 1;
}

//
// IO.File isFolder
//
SQInteger IOFileisFolder(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 1) {
        return sq_throwerror(vm, "too many parameters, expected at most 0");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "isFolder method needs an instance of File");
    }
    File *obj = static_cast<File*>(userPtr);
    if(!obj) {
        return sq_throwerror(vm, "isFolder method called before IO.File constructor");
    }
    // return value
    SQBool ret;
    // call the implementation
    try {
        ret = obj->isFolder();
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // push return value
    sq_pushbool(vm, ret);
    return 1;
}

//
// IO.File readAll
//
SQInteger IOFilereadAll(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 1) {
        return sq_throwerror(vm, "too many parameters, expected at most 0");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "readAll method needs an instance of File");
    }
    File *obj = static_cast<File*>(userPtr);
    if(!obj) {
        return sq_throwerror(vm, "readAll method called before IO.File constructor");
    }
    // return value
    const SQChar* ret;
    // call the implementation
    try {
        ret = obj->readAll();
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // push return value
    sq_pushstring(vm, ret, strlen(ret));
    return 1;
}

//
// IO.File size
//
SQInteger IOFilesize(HSQUIRRELVM vm)
{
    SQInteger numargs = sq_gettop(vm);
    // check parameter count
    if(numargs > 1) {
        return sq_throwerror(vm, "too many parameters, expected at most 0");
    }
    // get "this" pointer
    SQUserPointer userPtr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, 1, &userPtr, 0))) {
        return sq_throwerror(vm, "size method needs an instance of File");
    }
    File *obj = static_cast<File*>(userPtr);
    if(!obj) {
        return sq_throwerror(vm, "size method called before IO.File constructor");
    }
    // return value
    SQInteger ret;
    // call the implementation
    try {
        ret = obj->size();
    }
    catch(std::exception const& e) {
        return sq_throwerror(vm, e.what());
    }

    // push return value
    sq_pushinteger(vm, ret);
    return 1;
}


void bindIO(HSQUIRRELVM vm)
{
    // create package table
    sq_pushstring(vm, "IO", -1);
    sq_newtable(vm);

    // create class IO.File
    sq_pushstring(vm, "File", -1);
    sq_newclass(vm, false);
    sq_getstackobj(vm, -1, &IOFileObject);
    sq_settypetag(vm, -1, &IOFileObject);

    // ctor for class File
    sq_pushstring(vm, _SC("constructor"), -1);
    sq_newclosure(vm, &IOFileCtor, 0);
    sq_newslot(vm, -3, false);

    // clone for class File
    sq_pushstring(vm, _SC("_cloned"), -1);
    sq_newclosure(vm, &IOFileClone, 0);
    sq_newslot(vm, -3, false);

    // methods for class File
    sq_pushstring(vm, _SC("exists"), -1);
    sq_newclosure(vm, &IOFileexists, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("isFolder"), -1);
    sq_newclosure(vm, &IOFileisFolder, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("readAll"), -1);
    sq_newclosure(vm, &IOFilereadAll, 0);
    sq_newslot(vm, -3, false);

    sq_pushstring(vm, _SC("size"), -1);
    sq_newclosure(vm, &IOFilesize, 0);
    sq_newslot(vm, -3, false);

    // push File to IO package table
    sq_newslot(vm, -3, false);

    // push package "IO" to root table
    sq_newslot(vm, -3, false);
}
}}