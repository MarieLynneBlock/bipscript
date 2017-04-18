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

#include "miditune.h"
#include <vector>
#include <map>

namespace bipscript {
namespace midi {

struct ABCError
{
    const char *mesg;
    int lineNo;
    int linePos;
    ABCError(const char *msg, int lineno, int linepos) :
        mesg(msg), lineNo(lineno), linePos(linepos) {}
};

class ABCReader
{
    static ABCReader *activeParser;
    uint32_t ticksPerQuarter;
    uint32_t ticksPerBeat;
    uint32_t beatsPerBar;
    uint32_t beatUnit;
    uint32_t activeTrack;
    Position currentPosition;
    std::map<int, Note> activeNote;
    std::map<int, Position> noteStart;
    std::vector<Tune*> tunes;
    std::vector<ABCError> errors;
    bool verbose; // TODO: parameterize
    std::string error();
    Tune *currentTune() { return tunes.back(); }
public:
    ABCReader() : beatsPerBar(4), beatUnit(4), verbose(true) {}
    static ABCReader *getActiveParser() {
        return activeParser;
    }
    Pattern *read(const char *abc, const char *key, const char *noteLength,
                  const char *meter, const char *rhythm);
    Pattern *read(const char *abc, const char *key, const char *noteLength,
                  const char *meter) {
        return read(abc, key, noteLength, meter, 0);
    }
    Pattern *read(const char *abc, const char *key, const char *noteLength);
    Pattern *read(const char *abc, const char *key) {
        return read(abc, key, "1/8");
    }
    Pattern *read(const char *abc) {
        return read(abc, "C");
    }
    Tune *readTune(const char *abc);
    void startTrack(uint32_t track);
    void startSequence(int format, int ntracks, int division);
    // callbacks
    void writeTempo(long tempo);
    int writeMetaEvent(long delta_time, int type, char *data, int size);
    int writeMidiEvent(long delta_time, int type, int chan, char *data, int size);
    void singleNoteTuningChange(int key, float midipitch);
    void addError(const char *mesg, int lineno, int linepos);
    void addWarning(const char *mesg, int lineno, int linepos);
};

}}

#endif // ABCREADER_H
