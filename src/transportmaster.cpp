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

#include "transportmaster.h"
#include "audioengine.h"

#include <cmath>

TransportMaster::~TransportMaster()
{
    AudioEngine::instance().releaseTransportMaster();
}

void TransportMaster::forceBeat(double bpm)
{
    this->bpm = bpm;
    doForceBeat = true;
}

void TransportMaster::setTime(jack_transport_state_t state, jack_nframes_t nframes, jack_position_t *pos, int new_pos)
{

    pos->valid = JackPositionBBT;
    pos->beats_per_bar = beatsPerBar;
    pos->beat_type = beatUnit;
    pos->ticks_per_beat = ticksPerBeat;
    pos->beats_per_minute = bpm;

    if (new_pos) {
        // TODO: loop over tempo changes up to this point
        long tick = ticksPerBeat * bpm * pos->frame / ((double) pos->frame_rate * 60.0);
        long beat = tick / ticksPerBeat;

        pos->bar = beat / beatsPerBar;
        pos->beat = beat - (pos->bar * beatsPerBar) + 1;
        pos->tick = tick - (beat * ticksPerBeat);
        pos->bar_start_tick = pos->bar * beatsPerBar * ticksPerBeat;

        pos->bar++; // bar is 1-based
        lastTick = tick;
    } else {

        lastTick += ticksPerBeat * bpm * nframes / ((double) pos->frame_rate * 60);
        pos->tick = std::lround(lastTick);

        while (pos->tick >= ticksPerBeat) {
            pos->tick -= ticksPerBeat;
            lastTick -= ticksPerBeat;
            if (++pos->beat > beatsPerBar) {
                pos->beat = 1;
                pos->bar++;
                pos->bar_start_tick += (beatsPerBar * ticksPerBeat);
            }
        }
    }

    if(doForceBeat) {
        if(pos->tick > ticksPerBeat / 2) {
            if (++pos->beat > beatsPerBar) {
                pos->beat = 1;
                pos->bar++;
                pos->bar_start_tick += (beatsPerBar * ticksPerBeat);
            }
        }
        lastTick = 0;
        pos->tick = 0;
        doForceBeat = false;
    }
}

/**
 * runs in script thread
 */
TransportMaster *TransportMasterCache::getTransportMaster(float bpm)
{
    TransportMaster *cached = cachedMaster.load();
    if(cached) {
        cached->setBpm(bpm);
    } else {
        cached = AudioEngine::instance().getTransportMaster(bpm);
        cachedMaster.store(cached);
    }
    active = true;
    return cached;
}

/**
 * runs in process thread
 */
void TransportMasterCache::reposition()
{
    TransportMaster *cached = cachedMaster.load();
    if(cached) {
        cached->reposition();
    }
}

/**
 * runs in script thread
 */
bool TransportMasterCache::scriptComplete()
{
    TransportMaster *cached = cachedMaster.load();
    if(cached && !active) {
        cachedMaster.store(0);
        delete cached;
    }
    active = false;
    return false;
}

