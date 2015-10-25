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
        if(index == BT_HOP_SIZE) {
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

void MidiBeatTracker::process(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time)
{
    EventConnection *connection = midiInput.load();
    uint32_t eventCount = 0;
    if(connection) {
        connection->process(rolling, pos, nframes, time);
        eventCount = connection->getEventCount();
    }

    // loop over frames
    MidiEvent *nextEvent = eventCount ? connection->getEvent(0) : 0;
    uint32_t eventIndex = 1;
    for(jack_nframes_t i = 0; i < nframes; i++) {

        // add any events at this frame
        while(nextEvent && nextEvent->getFrameOffset() == i) {
            currentOnset += nextEvent->getDatabyte2(); // TODO: different weights for different notes
            nextEvent = eventIndex < eventCount ? connection->getEvent(eventIndex++) : 0;
        }

        if(frameIndex == BT_HOP_SIZE) {
            // process onset sample
            btrack.processOnsetDetectionFunctionSample(currentOnset);
            if(btrack.beatDueInCurrentFrame()) {
                // set bpm
                master->forceBeat(btrack.getCurrentTempoEstimate());
            }
            frameIndex = 0;
            currentOnset = 0;
        }
        frameIndex++;
    }
}

void MidiBeatTracker::reset(double bpm)
{
    master = TransportMasterCache::instance().getTransportMaster(bpm);
    btrack.setTempo(bpm);
}

/**
 * runs in script thread
 */
MidiBeatTracker *MidiBeatTrackerCache::getMidiBeatTracker(float bpm)
{
    MidiBeatTracker *cached = cachedTracker.load();
    if(cached) {
        cached->reset(bpm);
    } else {
        cached = new MidiBeatTracker(bpm);
        cachedTracker.store(cached);
    }
    active = true;
    return cached;
}


void MidiBeatTrackerCache::process(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time)
{
    MidiBeatTracker *cached = cachedTracker.load();
    if(cached) {
        cached->process(rolling, pos, nframes, time);
    }
}

/**
 * runs in process thread
 */
void MidiBeatTrackerCache::reposition()
{
    MidiBeatTracker *cached = cachedTracker.load();
    if(cached) {
        cached->reposition();
    }
}

/**
 * runs in script thread
 */
bool MidiBeatTrackerCache::scriptComplete()
{
    MidiBeatTracker *cached = cachedTracker.load();
    if(cached && !active) {
        cachedTracker.store(0);
        delete cached;
    }
    active = false;
    return false;
}
