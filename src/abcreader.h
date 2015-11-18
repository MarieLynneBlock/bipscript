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
#ifndef ABCREADER_H
#define ABCREADER_H

#include <string>
#include "miditune.h"

class ABCReader
{
    static ABCReader *activeParser;
    MidiTune *tune;
    uint32_t ticksPerBeat;
    uint32_t beatsPerBar;
    uint32_t activeTrack;
    Position currentPosition;
public:
    ABCReader() : tune(0), beatsPerBar(4) {}
    static ABCReader *getActiveParser() {
        return activeParser;
    }
    MidiTune *read(const char *abc);
    void startTrack(uint32_t track);
    void startSequence(int format, int ntracks, int division);
    // callbacks
    void writeTempo(long tempo);
    int writeMetaEvent(long delta_time, int type, char *data, int size);
    int writeMidiEvent(long delta_time, int type, int chan, char *data, int size);
    void singleNoteTuningChange(int key, float midipitch);
};

#endif // ABCREADER_H
