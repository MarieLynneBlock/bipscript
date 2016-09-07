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

#include "bindtypes.h"

#include "audioport.h"
#include "bindaudio.h"
#include "bindlv2.h"
#include "bindmidi.h"
#include "lv2plugin.h"
#include "midiport.h"
#include "mixer.h"

namespace binding
{

    AudioSource *getAudioSource(HSQUIRRELVM &vm) {
        SQUserPointer sourcePtr = 0;
        if (!SQ_FAILED(sq_getinstanceup(vm, 2, (SQUserPointer*)&sourcePtr, &AudioMixerObject))) {
            return static_cast<Mixer*>(sourcePtr);
        }
        if (!SQ_FAILED(sq_getinstanceup(vm, 2, (SQUserPointer*)&sourcePtr, &AudioInputObject))) {
            return static_cast<AudioInputPort*>(sourcePtr);
        }
        if (!SQ_FAILED(sq_getinstanceup(vm, 2, (SQUserPointer*)&sourcePtr, &AudioStereoInputObject))) {
            return static_cast<AudioStereoInput*>(sourcePtr);
        }
        if (!SQ_FAILED(sq_getinstanceup(vm, 2, (SQUserPointer*)&sourcePtr, &Lv2PluginObject))) {
            return static_cast<Lv2Plugin*>(sourcePtr);
        }
        return 0;
    }
    
    EventSource *getEventSource(HSQUIRRELVM &vm) {
        SQUserPointer sourcePtr = 0;
        if (!SQ_FAILED(sq_getinstanceup(vm, 2, (SQUserPointer*)&sourcePtr, &Lv2PluginObject))) {
            return static_cast<Lv2Plugin*>(sourcePtr);
        }
        if (!SQ_FAILED(sq_getinstanceup(vm, 2, (SQUserPointer*)&sourcePtr, &MidiInputObject))) {
            return static_cast<MidiInputPort*>(sourcePtr);
        }
        return 0;
    }
    
}