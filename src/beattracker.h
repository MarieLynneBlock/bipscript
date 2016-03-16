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
#include "eventconnection.h"
#include "objectcache.h"
#include "scripttypes.h"

const unsigned int BT_HOP_SIZE = 512;

class BeatTracker
{
    BTrack btrack;
    double *btbuffer;
    unsigned int index;
    TransportMaster *master;
    std::atomic<AudioConnection *> audioInput;
public:
    BeatTracker(double bpm, float beatsPerBar, float beatUnit)
        : index(0), audioInput(0) {
        btbuffer = new double[BT_HOP_SIZE];
        reset(bpm, beatsPerBar, beatUnit);
    }
    void connect(AudioSource &source) {
        this->audioInput.store(source.getAudioConnection(0));
    }
    void reset(double bpm, float beatsPerBar, float beatUnit);
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
    BeatTracker *getBeatTracker(float bpm, float beatsPerBar, float beatUnit);
    BeatTracker *getBeatTracker(float bpm, float beatsPerBar) {
        return getBeatTracker(bpm, beatsPerBar, 4);
    }
    BeatTracker *getBeatTracker(float bpm) {
        return getBeatTracker(bpm, 4);
    }
    // object cache interface
    void process(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time);
    void reposition();
    bool scriptComplete();
};

class OnCountClosure : public ScriptFunctionClosure {
    int count;
protected:
    bool addParameters() { addInteger(count); }
public:
    OnCountClosure(ScriptFunction function, int count) :
        ScriptFunctionClosure(function), count(count) {}
};

class MidiBeatTracker {
    BTrack btrack;
    uint32_t frameIndex;
    double currentOnset;
    TransportMaster *master;
    std::atomic<EventConnection*> midiInput;
    float noteWeight[128];
    // for count-in
    std::atomic<uint8_t> countInNote;
    uint8_t countInCount;
    jack_nframes_t lastCountTime[4];
    jack_nframes_t countStartTime;
    std::atomic<ScriptFunction*> onCountHandler;
    // for stopOnSilence
    std::atomic<uint32_t> stopSeconds;
    jack_nframes_t lastEventTime;
public:
    MidiBeatTracker(double bpm, float beatsPerBar, float beatUnit)
        : frameIndex(0), currentOnset(0), countInNote(0),
          countInCount(0), onCountHandler(0), lastEventTime(0) {
        reset(bpm, beatsPerBar, beatUnit);
    }
    void connectMidi(EventSource &source) {
        this->midiInput.store(&source.getEventConnection(0));
    }
    void setNoteWeight(uint32_t note, float weight);
    void countIn(uint8_t note) { countInNote.store(note); }
    void onCount(ScriptFunction &handler);
    void stopOnSilence(uint32_t seconds) { stopSeconds.store(seconds); }
    void reset(double bpm, float beatsPerBar, float beatUnit);
    void reposition() {}
    void process(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time);
private:
    void dispatchCountInEvent(uint32_t count);
    void detectCountIn(jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time, uint32_t eventCount, EventConnection *connection);
    void countInEvent(MidiEvent *nextEvent, jack_position_t &pos, jack_nframes_t time);
    void stopIfSilent(bool rolling, jack_position_t &pos, jack_nframes_t time);
};

class MidiBeatTrackerCache : public ObjectCache {
    bool active;
    std::atomic<MidiBeatTracker *> cachedTracker;
    MidiBeatTrackerCache() : active(false) {}
public:
    static MidiBeatTrackerCache &instance() {
        static MidiBeatTrackerCache instance;
        return instance;
    }
    MidiBeatTracker *getMidiBeatTracker(float bpm, float beatsPerBar, float beatUnit);
    MidiBeatTracker *getMidiBeatTracker(float bpm, float beatsPerBar) {
        return getMidiBeatTracker(bpm, beatsPerBar, 4);
    }
    MidiBeatTracker *getMidiBeatTracker(float bpm) {
        return getMidiBeatTracker(bpm, 4);
    }
    // object cache interface
    void process(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time);
    void reposition();
    bool scriptComplete();
};


#endif // BEATTRACKER_H
