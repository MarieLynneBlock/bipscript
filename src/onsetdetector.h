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
#ifndef ONSETDETECTOR_H
#define ONSETDETECTOR_H

#include "scripttypes.h"
#include "audioconnection.h"
#include "objectcache.h"
#include "aubio/aubio.h"

const unsigned int ONSET_HOP_SIZE = 512;

class OnsetDetector
{
    aubio_onset_t *aubioOnset;
    fvec_t *aubioInput;
    fvec_t *aubioOutput;
    unsigned int index;
    std::atomic<AudioConnection *> audioInput;
    std::atomic<ScriptFunction*> onOnsetHandler;
public:
    OnsetDetector(const char *type);
    ~OnsetDetector();
    void onOnset(ScriptFunction &handler);
    void connect(AudioSource &source) {
        this->audioInput.store(source.getAudioConnection(0));
    }
    void setThreshold(smpl_t threshold) {
        aubio_onset_set_threshold(aubioOnset, threshold);
    }
    void setSilence(smpl_t silence) {
        aubio_onset_set_silence(aubioOnset, silence);
    }
    void reset(const char *type);
    void process(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time);
};

class OnOnsetClosure : public ScriptFunctionClosure {
    int count;
protected:
     bool addParameters() { addInteger(count); }
public:
    OnOnsetClosure(ScriptFunction function, int count) :
        ScriptFunctionClosure(function), count(count) {}
};

class OnsetDetectorCache : public ObjectCache
{
    bool active;
    std::atomic<OnsetDetector *> cachedDetector;
    OnsetDetectorCache() : active(false) {}
public:
    static OnsetDetectorCache &instance() {
        static OnsetDetectorCache instance;
        return instance;
    }
    OnsetDetector *getOnsetDetector(const char *type);
    OnsetDetector *getOnsetDetector() {
        return getOnsetDetector("default");
    }
    // object cache interface
    void process(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time);
    void reposition() {}
    bool repositionComplete() { return true; }
    bool scriptComplete();
};


#endif // ONSETDETECTOR_H
