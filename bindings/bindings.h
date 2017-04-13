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
#ifndef BINDINGS_H
#define BINDINGS_H

#include "bindaudio.h"
#include "bindio.h"
#include "bindlv2.h"
#include "bindmath.h"
#include "bindmidi.h"
#include "bindosc.h"
#include "bindsystem.h"
#include "bindtransport.h"

namespace bipscript {
namespace binding {

void bindAll(HSQUIRRELVM vm)
{
    bindAudio(vm);
    bindIO(vm);
    bindLv2(vm);
    bindMath(vm);
    bindMidi(vm);
    bindOsc(vm);
    bindSystem(vm);
    bindTransport(vm);
}

}}

#endif // BINDINGS_H