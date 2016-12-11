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


OnsetDetector::OnsetDetector() :
    odf(512,1024,ComplexSpectralDifferenceHWR,HanningWindow),
    index(0), thold(1.0), lastOnsetFrame(0), audioInput(0)
{
    buffer = new double[ONSET_HOP_SIZE];
    for(int i = 0; i < HISTORY_SIZE; i++) {
        history[i] = 0.0;
    }
}

OnsetDetector::~OnsetDetector()
{
    delete[] buffer;
}

void OnsetDetector::onOnset(ScriptFunction &handler)
{
    if(handler.getNumargs() != 2) {
        throw std::logic_error("onOnset handler should take one argument");
    }
    onOnsetHandler.store(new ScriptFunction(handler));
}

void OnsetDetector::reset()
{    
    // TODO: implement type change?
    if(onOnsetHandler) {
        onOnsetHandler.load()->release();
        delete onOnsetHandler;
        onOnsetHandler = 0;
    }
}

void OnsetDetector::doProcess(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time)
{
    // get incoming audio
    AudioConnection *connection = audioInput.load();
    float *audio;
    if(connection) {
	connection->getSource()->process(rolling, pos, nframes, time);
        audio = connection->getAudio();
    } else {
        audio = AudioConnection::getDummyBuffer();
    }

    // add new audio to buffer
    for(jack_nframes_t i = 0; i < nframes; i++) {
        buffer[index++] = audio[i];
        if(index == ONSET_HOP_SIZE) {
            // calculate history median
            double sorted[HISTORY_SIZE];
            for(int i = 0; i < HISTORY_SIZE; i++) {
                sorted[i] = history[i];
            }
            std::sort(sorted, sorted + HISTORY_SIZE);
            double median = sorted[(HISTORY_SIZE / 2) + 1];
            // process full buffer
            double onset = odf.calculateOnsetDetectionFunctionSample(buffer);
            // calculate time since last onset
            jack_nframes_t sampleRate = AudioEngine::instance().getSampleRate();
            float delta = (time - lastOnsetFrame) / (float)sampleRate;
            // fire if ready
            // * onset above local median by threshold amount
            // * long enough period elapsed since last onset
            float fire = thold * 150;
            if(onset - median >= fire && delta > 0.025) {
                ScriptFunction *handler = onOnsetHandler.load();
                if(handler) {
                    // TODO: compute position from interpolation?
                    TimePosition position(pos);
                    (new OnOnsetClosure(*handler, position))->dispatch();
                }
                lastOnsetFrame = time;
            }
            // shift history samples one unit down
            for(int i = 0; i < HISTORY_SIZE - 1; i++) {
                history[i] = history[i + 1];
            }
            history[HISTORY_SIZE - 1] = onset;
            index = 0;
        }
    }
}


/**
 * runs in script thread
 */
OnsetDetector *OnsetDetectorCache::getOnsetDetector()
{
    OnsetDetector *detector = findObject(1); // TODO: allow multiple?
    if(!detector) {
        detector = new OnsetDetector();
        registerObject(1, detector);
    } else {
        detector->reset();
    }
    return detector;
}
