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

namespace bipscript {
namespace binding {


    audio::Source *getAudioSource(HSQUIRRELVM &vm, int index) {
        SQUserPointer sourcePtr;
        if (!SQ_FAILED(sq_getinstanceup(vm, index, (SQUserPointer*)&sourcePtr, &AudioMixerObject))) {
            return static_cast<audio::Mixer*>(sourcePtr);
        }
        if (!SQ_FAILED(sq_getinstanceup(vm, index, (SQUserPointer*)&sourcePtr, &AudioSystemInObject))) {
            return static_cast<audio::AudioInputPort*>(sourcePtr);
        }
        if (!SQ_FAILED(sq_getinstanceup(vm, index, (SQUserPointer*)&sourcePtr, &AudioStereoInObject))) {
            return static_cast<audio::AudioStereoInput*>(sourcePtr);
        }
        if (!SQ_FAILED(sq_getinstanceup(vm, index, (SQUserPointer*)&sourcePtr, &Lv2PluginObject))) {
            return static_cast<lv2::Plugin*>(sourcePtr);
        }
        return 0;
    }
    
    midi::Message *getMidiMessage(HSQUIRRELVM &vm, int index) {
        SQUserPointer sourcePtr;
        if (!SQ_FAILED(sq_getinstanceup(vm, index, (SQUserPointer*)&sourcePtr, &MidiNoteOnObject))) {
            return static_cast<midi::NoteOn*>(sourcePtr);
        }
        if (!SQ_FAILED(sq_getinstanceup(vm, index, (SQUserPointer*)&sourcePtr, &MidiNoteOffObject))) {
            return static_cast<midi::NoteOff*>(sourcePtr);
        }
        if (!SQ_FAILED(sq_getinstanceup(vm, index, (SQUserPointer*)&sourcePtr, &MidiControlObject))) {
            return static_cast<midi::Control*>(sourcePtr);
        }
        if (!SQ_FAILED(sq_getinstanceup(vm, index, (SQUserPointer*)&sourcePtr, &MidiPitchBendObject))) {
            return static_cast<midi::PitchBend*>(sourcePtr);
        }
        if (!SQ_FAILED(sq_getinstanceup(vm, index, (SQUserPointer*)&sourcePtr, &MidiProgramChangeObject))) {
            return static_cast<midi::ProgramChange*>(sourcePtr);
        }
        return 0;
    }
    
    midi::Source *getMidiSource(HSQUIRRELVM &vm, int index) {
        SQUserPointer sourcePtr;
        if (!SQ_FAILED(sq_getinstanceup(vm, index, (SQUserPointer*)&sourcePtr, &Lv2PluginObject))) {
            return static_cast<lv2::Plugin*>(sourcePtr);
        }
        if (!SQ_FAILED(sq_getinstanceup(vm, index, (SQUserPointer*)&sourcePtr, &MidiSystemInObject))) {
            return static_cast<midi::MidiInputPort*>(sourcePtr);
        }
        return 0;
    }
    
}}