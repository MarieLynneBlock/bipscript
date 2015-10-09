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
#ifndef TRANSPORT_H
#define TRANSPORT_H

#include "jack/types.h"
#include "objectcache.h"
#include <atomic>

class TransportMaster
{
    double lastTick;
    float beatsPerBar;
    float beatUnit;
    double ticksPerBeat;
    double bpm;
public:
    TransportMaster(double bpm) : beatsPerBar(4.0), beatUnit(4.0),
        ticksPerBeat(1920.0), bpm(bpm) {}
    ~TransportMaster();
    // void scheduleBpm(bpm, bar, position, division)
    void setBpm(double bpm) {
        this->bpm = bpm;
    }
    void setTime(jack_transport_state_t state, jack_nframes_t nframes, jack_position_t *pos, int new_pos);
    void reposition() {} // TODO: need this?
};

class TransportMasterCache : public ObjectCache
{
    bool active;
    std::atomic<TransportMaster *> cachedMaster;
    TransportMasterCache() : active(false) {}
public:
    static TransportMasterCache &instance() {
        static TransportMasterCache instance;
        return instance;
    }
    TransportMaster *getTransportMaster(float bpm);
    // object cache interface
    void process(bool, jack_position_t &, jack_nframes_t, jack_nframes_t) {}
    void reposition();
    bool scriptComplete();
};

#endif // TRANSPORT_H
