
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
#include "oscoutput.h"
#include "objectcollector.h"
#include "audioengine.h"

#include <jack/jack.h>
#include <chrono>
#include <thread>

void *run_output(void *arg)
{
    ((OscOutput*)arg)->run();
}

OscOutput::OscOutput(const char *host, int port) :
    repositionNeeded(false), cancelled(false)
{
    std::string portString = std::to_string(port);
    loAddress = lo_address_new(host, portString.c_str());
    if(!loAddress) {
        throw std::logic_error("could not create OSC address");
    }
    int result = pthread_create(&thread, 0, run_output, this);
    if(result) {
        throw std::logic_error("could not create OSC output thread");
    }
}

void OscOutput::schedule(OscMessage &message, int bar, int position, int division)
{
    Position pos(bar, position, division);
    eventBuffer.addEvent(new OscEvent(pos, message));
}

void OscOutput::run()
{
    while(!cancelled.load()) {
        // reset flag = dump all events
        if(repositionNeeded.load()) {
            eventBuffer.recycleRemaining();
            repositionNeeded.store(false);
        }
        // get current audio position
        jack_position_t jack_pos;
        bool rolling = AudioEngine::instance().getPosition(jack_pos);

        // calculate window size
        jack_nframes_t nframes = 2048; // TODO: calculate from framerate

        OscEvent *event = eventBuffer.getNextEvent(rolling, jack_pos, nframes);
        while(event) {
            OscMessage &message = event->getMessage();
            lo_message mesg = lo_message_new();
            for(int i = 0; i < message.getParameterCount(); i++) {
                OscParameter param = message.getParameter(i);
                if(param.type == 'i') {
                    lo_message_add_int32(mesg, param.value.intValue);
                } else if(param.type == 'f') {
                    lo_message_add_float(mesg, param.value.floatValue);
                } else if(param.type == 's') {
                    lo_message_add_string(mesg, param.value.stringValue);
                } else if(param.type == 'T') {
                    lo_message_add_true(mesg);
                } else if(param.type == 'F') {
                    lo_message_add_false(mesg);
                } else if(param.type == 'N') {
                    lo_message_add_nil(mesg);
                }
            }
            lo_send_message (loAddress, event->getMessage().getPath(), mesg);
            lo_message_free (mesg);
            ObjectCollector::scriptCollector().recycle(event);
            event = eventBuffer.getNextEvent(rolling, jack_pos, nframes);
        }

        // TODO: better timing
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    // we're done with the entire object
    AudioEngine::instance().removeProcessor(this);
}

/**
 * script thread
 */
OscOutput *OscOutputFactory::getOscOutput(const char *host, int port)
{
    // TODO: needs better hash
    int key = std::hash<std::string>()(host) + port;
    OscOutput *obj = findObject(key);
    if (!obj) {
        obj = new OscOutput(host, port);
        registerObject(key, obj);
    }
    return obj;
}
