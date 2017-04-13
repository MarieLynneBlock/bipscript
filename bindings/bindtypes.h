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

namespace audio { class AudioSource; }
namespace midi { class MidiMessage; }
namespace midi { class MidiSource; }
namespace midi { class MidiSink; }

namespace binding
{
    // type conversion methods
    audio::AudioSource *getAudioSource(HSQUIRRELVM &vm, int index);
    midi::MidiMessage *getMidiMessage(HSQUIRRELVM &vm, int index);
    midi::MidiSource *getMidiSource(HSQUIRRELVM &vm, int index);
    midi::MidiSink *getMidiSink(HSQUIRRELVM &vm, int index);
}
}
#endif // BINDTYPES_H