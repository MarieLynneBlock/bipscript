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

#include "audioconnection.h"
#include "objectcache.h"
#include "eventclosure.h"
#include "OnsetDetectionFunction.h"
#include "timeposition.h"
#include "bindtransport.h"

namespace bipscript {
namespace audio {

const unsigned int ONSET_HOP_SIZE = 512;
const unsigned int HISTORY_SIZE = 8;

class OnsetDetector : public Processor
{
    OnsetDetectionFunction odf;
    unsigned int index;
    double *buffer;
    double history[HISTORY_SIZE];
    float thold;
    jack_nframes_t lastOnsetFrame;
    std::atomic<AudioConnection *> audioInput;
    std::atomic<ScriptFunction*> onOnsetHandler;
public:
    OnsetDetector();
    ~OnsetDetector();
    void onOnset(ScriptFunction &handler);
    void connect(AudioSource &source) {
        this->audioInput.store(source.getAudioConnection(0));
    }
    void connect(AudioConnection &connection) {
        this->audioInput.store(&connection);
    }
    float threshold() {
        return thold;
    }
    float threshold(float threshold) {
        return thold = threshold;
    }
    void reset();
    void doProcess(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time);
    void reposition() {}
};

class OnOnsetClosure : public EventClosure {
    transport::TimePosition pos;
protected:
     void addParameters() { addObject(&pos, binding::TransportPositionObject); }
public:
    OnOnsetClosure(ScriptFunction function, transport::TimePosition &pos) :
        EventClosure(function), pos(pos) {}
};

class OnsetDetectorCache : public ProcessorCache<OnsetDetector>
{
public:
    static OnsetDetectorCache &instance() {
        static OnsetDetectorCache instance;
        return instance;
    }
    OnsetDetector *getOnsetDetector();
};

}}

#endif // ONSETDETECTOR_H
