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
#ifndef MIDICONNECTION_H
#define MIDICONNECTION_H

#include <atomic>
#include <jack/types.h>
#include "source.h"
#include "midievent.h"
#include "eventclosure.h"
#include "midimessage.h"
#include "bindmidi.h"
#include "bindtransport.h"
#include "timeposition.h"

namespace bipscript {
namespace midi {

class MidiControlEventClosure : public EventClosure {
    Control control;
    transport::TimePosition position;
protected:
    void addParameters() {
        binding::MidiControlPush(vm, &control);
        binding::TransportPositionPush(vm, &position);
    }
public:
    MidiControlEventClosure(ScriptFunction function,
                            Control &control, transport::TimePosition &position)
      : EventClosure(function), control(control), position(position) {}
};

class MidiNoteOnEventClosure : public EventClosure {
    NoteOn noteOn;
    transport::TimePosition position;
protected:
    void addParameters() {
        binding::MidiNoteOnPush(vm, &noteOn);
        binding::TransportPositionPush(vm, &position);
    }
public:
    MidiNoteOnEventClosure(ScriptFunction function,
                            NoteOn &noteOn, transport::TimePosition &position)
      : EventClosure(function), noteOn(noteOn), position(position) {}
};

class MidiNoteOffEventClosure : public EventClosure {
    NoteOff noteOff;
    transport::TimePosition position;
protected:
    void addParameters() {
        binding::MidiNoteOffPush(vm, &noteOff);
        binding::TransportPositionPush(vm, &position);
    }
public:
    MidiNoteOffEventClosure(ScriptFunction function,
                            NoteOff &noteOff, transport::TimePosition &position)
      : EventClosure(function), noteOff(noteOff), position(position) {}
};

class MidiSource;

class MidiConnection {
    MidiSource *source;
    std::atomic<bool> handlerDefined;
    std::atomic<ScriptFunction*> onControlHandler;
    std::atomic<ScriptFunction*> onNoteOnHandler;
    std::atomic<ScriptFunction*> onNoteOffHandler;
public:
    MidiConnection(MidiSource *source) :
      source(source), handlerDefined(false), onControlHandler(0),
      onNoteOnHandler(0), onNoteOffHandler(0) {}
    MidiSource *getSource() { return source; }
    virtual uint32_t getEventCount() = 0;
    virtual MidiEvent *getEvent(uint32_t i) = 0;
    void onControl(ScriptFunction &handler) {
        if(handler.getNumargs() != 3) {
            throw std::logic_error("onControl handler should take two arguments");
        }
        onControlHandler.store(new ScriptFunction(handler));
        handlerDefined.store(true);
    }
    void onNoteOn(ScriptFunction &handler) {
        if(handler.getNumargs() != 3) {
            throw std::logic_error("onNoteOn handler should take two arguments");
        }
        onNoteOnHandler.store(new ScriptFunction(handler));
        handlerDefined.store(true);
    }
    void onNoteOff(ScriptFunction &handler) {
        if(handler.getNumargs() != 3) {
            throw std::logic_error("onNoteOff handler should take two arguments");
        }
        onNoteOffHandler.store(new ScriptFunction(handler));
        handlerDefined.store(true);
    }
    void fireEvents(jack_position_t &pos) {
        if(handlerDefined.load()) {
            ScriptFunction *ccHandler = onControlHandler.load();
            ScriptFunction *onHandler = onNoteOnHandler.load();
            ScriptFunction *offHandler = onNoteOffHandler.load();
            for(int j = 0; j < getEventCount(); j++) {
                MidiEvent *evt = getEvent(j);
                if(evt->getType() == MidiEvent::TYPE_CONTROL && ccHandler) {
                    transport::TimePosition position(pos, evt->getFrameOffset());
                    Control control(evt->getDatabyte1(), evt->getDatabyte2());
                    (new MidiControlEventClosure(*ccHandler, control, position))->dispatch();
                }
                else if(evt->getType() == MidiEvent::TYPE_NOTE_ON && onHandler) {
                    transport::TimePosition position(pos, evt->getFrameOffset());
                    NoteOn noteOn(evt->getDatabyte1(), evt->getDatabyte2());
                    (new MidiNoteOnEventClosure(*onHandler, noteOn, position))->dispatch();
                }
                else if(evt->getType() == MidiEvent::TYPE_NOTE_OFF && offHandler) {
                    transport::TimePosition position(pos, evt->getFrameOffset());
                    NoteOff noteOff(evt->getDatabyte1(), evt->getDatabyte2());
                    (new MidiNoteOffEventClosure(*offHandler, noteOff, position))->dispatch();
                }
            }
        }
    }
};

class MidiSource : virtual public Source
{
public:
    virtual unsigned int getMidiOutputCount() = 0;
    virtual MidiConnection *getMidiConnection(unsigned int index) = 0;
    void onControl(ScriptFunction &handler) {
        for(int i = 0; i < getMidiOutputCount(); i++) {
            getMidiConnection(i)->onControl(handler);
        }
    }
    void onNoteOn(ScriptFunction &handler) {
        for(int i = 0; i < getMidiOutputCount(); i++) {
            getMidiConnection(i)->onNoteOn(handler);
        }
    }
    void onNoteOff(ScriptFunction &handler) {
        for(int i = 0; i < getMidiOutputCount(); i++) {
            getMidiConnection(i)->onNoteOff(handler);
        }
    }
protected:
    void fireMidiEvents(jack_position_t &pos) {
        for(int i = 0; i < getMidiOutputCount(); i++) {
            getMidiConnection(i)->fireEvents(pos);
        }
    }
};

class MidiConnector {
    std::atomic<MidiConnection *> connection;
public:
    MidiConnector() : connection(0) {}
    void setConnection(MidiConnection *conn, Source *source) {
        Source *connSource = conn->getSource();
        if(connSource == source || connSource->connectsTo(source)) {
            throw std::logic_error("Cannot connect infinite loop");
        }
        connection.store(conn);
    }
    MidiConnection *getConnection() {
        return connection.load();
    }
};

}}

#endif // MIDICONNECTION_H
