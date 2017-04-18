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
#ifndef BINDOSC_H
#define BINDOSC_H

#include "squirrel.h"

namespace bipscript {

namespace osc {
class Message;
}

namespace binding
{
    // object references to types in this package
    extern HSQOBJECT OscInputObject;
    extern HSQOBJECT OscMessageObject;
    extern HSQOBJECT OscOutputObject;
    osc::Message *getOscMessage(HSQUIRRELVM &vm, int index);
    // release hooks for types in this package
    SQInteger OscMessageRelease(SQUserPointer p, SQInteger size);
    // method to bind this package
    void bindOsc(HSQUIRRELVM vm);
}}

#endif // BINDOSC_H