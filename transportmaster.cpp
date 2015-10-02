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

void TransportMaster::setTime(jack_transport_state_t state, jack_nframes_t nframes, jack_position_t *pos, int new_pos)
{
    if (new_pos) {

        pos->valid = JackPositionBBT;
        pos->beats_per_bar = beatsPerBar;
        pos->beat_type = beatUnit;
        pos->ticks_per_beat = ticksPerBeat;
        pos->beats_per_minute = bpm;

        // simple calculation from examples
        double min = pos->frame / ((double) pos->frame_rate * 60.0);
        long  tick = min * pos->beats_per_minute * pos->ticks_per_beat;
        long beat = tick / pos->ticks_per_beat;

        pos->bar = beat / pos->beats_per_bar;
        pos->beat = beat - (pos->bar * pos->beats_per_bar) + 1;
        pos->tick = tick - (beat * pos->ticks_per_beat);
        pos->bar_start_tick = pos->bar * pos->beats_per_bar * pos->ticks_per_beat;

        pos->bar++; // bar is 1-based

    } else {

        // from examples: compute based on previous period
        pos->tick += (nframes * pos->ticks_per_beat *
                  pos->beats_per_minute / (pos->frame_rate * 60));

        while (pos->tick >= pos->ticks_per_beat) {
            pos->tick -= pos->ticks_per_beat;
            if (++pos->beat > pos->beats_per_bar) {
                pos->beat = 1;
                ++pos->bar;
                pos->bar_start_tick += (pos->beats_per_bar * pos->ticks_per_beat);
            }
        }
    }
}

