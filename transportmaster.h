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

class TransportMaster
{
    float beatsPerBar;
    float beatUnit;
    double ticksPerBeat;
    double bpm;
public:
    TransportMaster(double bpm) : beatsPerBar(4.0), beatUnit(4.0),
        ticksPerBeat(1920.0), bpm(bpm) {}
    // void scheduleBpm(bpm, bar, position, division)
    void setTime(jack_transport_state_t state, jack_nframes_t nframes, jack_position_t *pos, int new_pos);
};

#endif // TRANSPORT_H
