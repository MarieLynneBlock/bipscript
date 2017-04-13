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
#include "timesignature.h"
#include <atomic>

namespace bipscript {
namespace transport {

class TransportMaster
{
    double lastTick;
    float beatsPerBar;
    float beatUnit;
    double ticksPerBeat;
    double bpm;
    bool doForceBeat;
public:
    TransportMaster(double bpm, float beatsPerBar, float beatUnit) :
        beatsPerBar(beatsPerBar), beatUnit(beatUnit),
        ticksPerBeat(1920.0), bpm(bpm), doForceBeat(false) {}
    ~TransportMaster();
    // void scheduleBpm(bpm, bar, position, division)
    void setBpm(double bpm) {
        this->bpm = bpm;
    }
    void forceBeat(double bpm);
    void setTimeSignature(uint32_t numerator, uint32_t denominator) {
        this->beatsPerBar = numerator;
        this->beatUnit = denominator;
    }
    void setTimeSignature(TimeSignature &timeSignature) {
        this->beatsPerBar = timeSignature.getNumerator();
        this->beatUnit = timeSignature.getDenominator();
    }
    void setTime(jack_transport_state_t state, jack_nframes_t nframes, jack_position_t *pos, int new_pos);
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
    TransportMaster *getTransportMaster(float bpm, float beatsPerBar, float beatUnit);
    TransportMaster *getTransportMaster(float bpm, float beatsPerBar) {
        return getTransportMaster(bpm, beatsPerBar, 4);
    }
    TransportMaster *getTransportMaster(float bpm) {
        return getTransportMaster(bpm, 4);
    }
    // object cache interface
    bool scriptComplete();
};

}}

#endif // TRANSPORT_H
