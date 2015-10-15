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
#include "beattracker.h"

#include "audioengine.h"

void BeatTracker::reset(double bpm) {
    master = TransportMasterCache::instance().getTransportMaster(bpm);
    btrack.setTempo(bpm);
}

void BeatTracker::process(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time)
{
    // get incoming audio
    AudioConnection *connection = audioInput.load();
    float *audio;
    if(connection) {
        audio = connection->getAudio(rolling, pos, nframes, time);
    } else {
        audio = AudioConnection::getDummyBuffer();
    }

    // add new audio to buffer
    for(jack_nframes_t i = 0; i < nframes; i++) {
        btbuffer[index++] = (double) audio[i];
        if(index == BUFFER_SIZE) {
            // process buffer if full
            btrack.processAudioFrame(btbuffer);
            if(btrack.beatDueInCurrentFrame()) {
                // set bpm
                master->forceBeat(btrack.getCurrentTempoEstimate());
                // printf("beat, bpm is %f\n", btrack.getCurrentTempoEstimate());
            }
            index = 0;
        }
    }
}

/**
 * runs in script thread
 */
BeatTracker *BeatTrackerCache::getBeatTracker(float bpm)
{
    BeatTracker *cached = cachedTracker.load();
    if(cached) {
        cached->reset(bpm);
    } else {
        cached = new BeatTracker(bpm);
        cachedTracker.store(cached);
    }
    active = true;
    return cached;
}

void BeatTrackerCache::process(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time)
{
    BeatTracker *cached = cachedTracker.load();
    if(cached) {
        cached->process(rolling, pos, nframes, time);
    }
}

/**
 * runs in process thread
 */
void BeatTrackerCache::reposition()
{
    BeatTracker *cached = cachedTracker.load();
    if(cached) {
        cached->reposition();
    }
}

/**
 * runs in script thread
 */
bool BeatTrackerCache::scriptComplete()
{
    BeatTracker *cached = cachedTracker.load();
    if(cached && !active) {
        cachedTracker.store(0);
        delete cached;
    }
    active = false;
    return false;
}

