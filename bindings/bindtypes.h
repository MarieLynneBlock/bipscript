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
#ifndef BINDTYPES_H
#define BINDTYPES_H

#include "squirrel.h"

#include "mixer.h"
#include "audioport.h"
#include "lv2plugin.h"
#include "midiport.h"

namespace binding
{
    // type conversion methods
    AudioSource *getAudioSource(SQUserPointer ptr, SQUserPointer typeTag);
    EventSource *getEventSource(SQUserPointer ptr, SQUserPointer typeTag);
}

#endif // BINDTYPES_H