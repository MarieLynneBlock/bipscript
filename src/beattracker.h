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
#ifndef BEATTRACKER_H
#define BEATTRACKER_H

#include <atomic>

#include "BTrack.h"
#include "transportmaster.h"
#include "audioconnection.h"
#include "objectcache.h"

class BeatTracker
{
    BTrack btrack;
    double *btbuffer;
    unsigned int index;
    TransportMaster *master;
    std::atomic<AudioConnection *> audioInput;
    const unsigned int BUFFER_SIZE = 512;
public:
    BeatTracker(double bpm) : index(0), audioInput(0) {
        btbuffer = new double[BUFFER_SIZE];
        reset(bpm);
    }
    void connect(AudioSource &source) {
        this->audioInput.store(&source.getAudioConnection(0));
    }
    void reset(double bpm);
    void reposition() {}
    void process(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time);
};

class BeatTrackerCache : public ObjectCache
{
    bool active;
    std::atomic<BeatTracker *> cachedTracker;
    BeatTrackerCache() : active(false) {}
public:
    static BeatTrackerCache &instance() {
        static BeatTrackerCache instance;
        return instance;
    }
    BeatTracker *getBeatTracker(float bpm);
    // object cache interface
    void process(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time);
    void reposition();
    bool scriptComplete();
};

#endif // BEATTRACKER_H
