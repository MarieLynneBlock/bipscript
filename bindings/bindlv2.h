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
#ifndef BINDLV2_H
#define BINDLV2_H

#include "squirrel.h"

namespace bipscript {

namespace lv2 {
class State;
}

namespace binding
{
    // object references to types in this package
    extern HSQOBJECT Lv2PluginObject;
    extern HSQOBJECT Lv2StateObject;
    lv2::State *getLv2State(HSQUIRRELVM &vm, int index);
    // release hooks for types in this package
    SQInteger Lv2StateRelease(SQUserPointer p, SQInteger size);
    // method to bind this package
    void bindLv2(HSQUIRRELVM vm);
}}

#endif // BINDLV2_H