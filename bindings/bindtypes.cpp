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

#include "bindaudio.h"
#include "bindlv2.h"
#include "bindmidi.h"

namespace binding
{

    AudioSource *getAudioSource(SQUserPointer ptr, SQUserPointer typeTag) {
        if(typeTag ==  &AudioMixerObject) {
            return static_cast<Mixer*>(ptr);
        }
        if(typeTag ==  &AudioInputObject) {
            return static_cast<AudioInputPort*>(ptr);
        }
        if(typeTag ==  &AudioStereoInputObject) {
            return static_cast<AudioStereoInput*>(ptr);
        }
        if(typeTag ==  &Lv2PluginObject) {
            return static_cast<Lv2Plugin*>(ptr);
        }        
        return 0;
    }
    
    EventSource *getEventSource(SQUserPointer ptr, SQUserPointer typeTag) {
        if(typeTag ==  &Lv2PluginObject) {
            return static_cast<Lv2Plugin*>(ptr);
        }
        if(typeTag ==  &MidiInputObject) {
            return static_cast<MidiInputPort*>(ptr);
        }        
        return 0;
    }
    
}