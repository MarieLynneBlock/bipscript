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

#include <iostream>

#include "audioengine.h"
#include "objectcollector.h"
#include "scripthost.h"
#include "audioconnection.h"
#include "transportmaster.h"
#include "lv2plugin.h"

using namespace std;

float *AudioConnection::dummyBuffer = 0;
jack_nframes_t AudioConnection::bufferSize;

void printPosition(jack_transport_state_t state, jack_position_t *pos, jack_nframes_t runningFrame)
{
    cout << "====" << endl << "sync: ";
    cout << "state is ";
    cout << (state == JackTransportStopped ? "STOPPED" : "");
    cout << (state == JackTransportStarting ? "STARTING" : "");
    cout << (state == JackTransportRolling ? "ROLLING" : "");
    cout << endl;
    cout << "frame is " << pos->frame << endl;
    cout << "ticks per beat is " << pos->ticks_per_beat << endl;
    if(pos->valid & JackPositionBBT) {
        std::cout << "BBT position is valid" << std::endl;
    }
    if(pos->valid & JackBBTFrameOffset) {
        std::cout << "frame offset is valid" << std::endl;
    }
    //if (state != JackTransportStopped) {
        cout << "bpm is " << pos->beats_per_minute << endl;
        cout << "beat is " << pos->beat << endl;
        cout << "bar is " << pos->bar << endl;
    //}
    if(pos->frame > runningFrame) {
        cout << "jumped ahead from " << runningFrame << " to " << pos->frame << endl;
    } else if(pos->frame == runningFrame) {
        cout << "no change, frame still " << runningFrame << endl;
    } else {
        cout << "rewinding from " << runningFrame << " back to " << pos->frame << endl;
    }
    cout << "====" << endl;
}

int jack_process(jack_nframes_t nframes, void *arg)
{
    return ((AudioEngine*)arg)->process(nframes);
}

int sync_callback(jack_transport_state_t state, jack_position_t *pos, void *arg)
{
    return ((AudioEngine*)arg)->sync(state, pos);
}

int buffersize_callback(jack_nframes_t nframes, void* arg)
{
    ((AudioEngine*)arg)->setBufferSize(nframes);
    return 0;
}

TimeSignature &AudioEngine::getTimeSignature()
{
    jack_position_t pos;
    jack_transport_query(client, &pos);
    bool valid = pos.valid & JackTransportBBT;
    currentTimeSignature = TimeSignature(valid, pos.beats_per_bar, pos.beat_type);
    return currentTimeSignature;
}

void AudioEngine::setBufferSize(jack_nframes_t size)
{
    // TODO: generic size listener?
    AudioConnection::setBufferSize(size);
    Lv2PluginCache::instance().setBufferSize(size);
}

int AudioEngine::activate(const char *clientName)
{
    // connect to Jack
    if ((client = jack_client_open(clientName, JackNullOption, NULL)) == 0) {
        return 1;
    }

    // get sample rate and buffer size
    sampleRate = jack_get_sample_rate(client);
    setBufferSize(jack_get_buffer_size(client));

    // Jack callbacks
    jack_set_process_callback(client, jack_process, this);
    jack_set_sync_callback(client, sync_callback, this);
    jack_set_buffer_size_callback(client, &buffersize_callback, this);

    // activate jack
    if (jack_activate(client)) {
        return 1;
    }

    return 0;
}

void AudioEngine::shutdown()
{
    jack_deactivate(client);
    jack_client_close(client);
}

void printPorts(jack_client_t *client) {
    const char** foo = jack_get_ports(client, 0, 0, 0 );
    for (int i = 0; foo && foo[i]; ++i) {
        cout << "- found port " << foo[i] << endl;
    }
}

void timebase_callback(jack_transport_state_t state, jack_nframes_t nframes,
     jack_position_t *pos, int new_pos, void *arg)
{
    ((TransportMaster*)arg)->setTime(state, nframes, pos, new_pos);
}

TransportMaster *AudioEngine::getTransportMaster(double bpm)
{
    if(!transportMaster) {
        transportMaster = new TransportMaster(bpm);
        int error = jack_set_timebase_callback(client, 0, &timebase_callback, transportMaster);
        if (error) {
            throw std::logic_error("Cannot create transport master");
        }
    }
    return transportMaster;
}

void AudioEngine::releaseTransportMaster()
{    
    jack_release_timebase(client);
    transportMaster = 0;
}

// from the API docs: TRUE (non-zero) when ready to roll
int AudioEngine::sync(jack_transport_state_t state, jack_position_t *pos)
{
    // currently in a restart
    if(multiplePeriodRestart) {
        if(!ScriptHost::instance().reposition(multiplePeriodRestart)) {
            multiplePeriodRestart++;
            return 0;
        }
        multiplePeriodRestart = 0;
        runningFrame = 0;
    }

    // if we've jumped backwards, need to reset
    else if(pos->frame < runningFrame) {
        if(!ScriptHost::instance().reposition(0)) {
            multiplePeriodRestart = 1;
            return 0;
        }
        // update runningFrame
        runningFrame = 0;
    }

    // printPosition(state, pos, runningFrame);

    // ready to roll
    return 1;
}

int AudioEngine::process(jack_nframes_t nframes)
{   
    // check jack transport state, are we rolling?
    jack_position_t pos;
    bool rolling(jack_transport_query(client, &pos) == JackTransportRolling);

    // update running position
    if(rolling) {
        runningFrame = pos.frame;
    }

    jack_nframes_t time = jack_last_frame_time(client);

    // process
    ScriptHost::instance().process(rolling, pos, nframes, time);

    // push out objects to delete
    ObjectCollector::instance()->update();

    return 0;
}
