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

namespace bipscript {

namespace audio { class Source; }
namespace midi { class Message; }
namespace midi { class Source; }

namespace binding
{
    // type conversion methods
    audio::Source *getAudioSource(HSQUIRRELVM &vm, int index);
    midi::Message *getMidiMessage(HSQUIRRELVM &vm, int index);
    midi::Source *getMidiSource(HSQUIRRELVM &vm, int index);
}
}
#endif // BINDTYPES_H