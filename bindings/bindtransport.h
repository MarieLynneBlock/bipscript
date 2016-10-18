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
#ifndef BINDTRANSPORT_H
#define BINDTRANSPORT_H

#include "squirrel.h"

namespace binding
{
    // object references to types in this package
    extern HSQOBJECT TransportMasterObject;
    extern HSQOBJECT TransportTimeSignatureObject;
    // release hooks for types in this package
    SQInteger TransportTimeSignatureRelease(SQUserPointer p, SQInteger size);
    // method to bind this package
    void bindTransport(HSQUIRRELVM vm);
}

#endif // BINDTRANSPORT_H