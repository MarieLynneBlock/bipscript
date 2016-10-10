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
#include "onsetdetector.h"
#include "audioengine.h"


OnsetDetector::OnsetDetector(const char *type) : index(0)
{
    jack_nframes_t sampleRate = AudioEngine::instance().getSampleRate();
    aubioOnset = new_aubio_onset (const_cast<char*>(type),
                                  ONSET_HOP_SIZE, ONSET_HOP_SIZE/2, sampleRate);
    aubioInput = new_fvec(ONSET_HOP_SIZE);
    aubioOutput = new_fvec(1);
}

OnsetDetector::~OnsetDetector()
{
    del_aubio_onset(aubioOnset);
}

void OnsetDetector::onOnset(ScriptFunction &handler)
{
    if(handler.getNumargs() != 2) {
        throw std::logic_error("onOnset handler should take one argument");
    }
    onOnsetHandler.store(new ScriptFunction(handler));
}

void OnsetDetector::reset(const char *type)
{    
    // TODO: implement type change?
    if(onOnsetHandler) {
        onOnsetHandler.load()->release();
        delete onOnsetHandler;
        onOnsetHandler = 0;
    }
}

void OnsetDetector::process(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time)
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
        aubioInput->data[index++] = (smpl_t) audio[i];
        if(index == ONSET_HOP_SIZE) {
            aubio_onset_do(aubioOnset, aubioInput, aubioOutput);
            smpl_t onset = fvec_get_sample(aubioOutput, 0);
            fvec_zeros(aubioOutput);
            if(onset) {
                ScriptFunction *handler = onOnsetHandler.load();
                if(handler) {
                    // TODO: compute position from smpl_t onset value
                    (new OnOnsetClosure(*handler, 0))->dispatch();
                }
            }
            index = 0;
        }
    }
}


/**
 * runs in script thread
 */
OnsetDetector *OnsetDetectorCache::getOnsetDetector(const char *type)
{
    OnsetDetector *cached = cachedDetector.load();
    if(cached) {
        cached->reset(type);
    } else {
        cached = new OnsetDetector(type);
        cachedDetector.store(cached);
    }
    active = true;
    return cached;
}


void OnsetDetectorCache::process(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time)
{
    OnsetDetector *cached = cachedDetector.load();
    if(cached) {
        cached->process(rolling, pos, nframes, time);
    }
}

/**
 * runs in script thread
 */
bool OnsetDetectorCache::scriptComplete()
{
    OnsetDetector *cached = cachedDetector.load();
    if(cached && !active) {
        cachedDetector.store(0);
        delete cached;
    }
    active = false;
    return false;
}

