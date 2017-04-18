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

#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <boost/filesystem.hpp>

#include "scripthost.h"
#include "audioengine.h"
#include "systempackage.h"

#include "lv2plugin.h"
#include "mixer.h"
#include "midiport.h"
#include "audioport.h"
#include "beattracker.h"
#include "onsetdetector.h"
#include "oscinput.h"
#include "oscoutput.h"
#include "extension.h"
#include "transport.h"

namespace fs = boost::filesystem;

using namespace bipscript;

void signal_handler(int sig)
{
    std::cerr << "caught signal " << sig << ", exiting" << std::endl;
    ExtensionManager::instance().shutdown();
    osc::OutputFactory::instance().shutdown();
    AudioEngine::instance().shutdown();
    exit(0);
}

int main(int argc, char **argv)
{
    if(argc < 2) {
        std::cerr << "Usage: bipscript [script file]" << std::endl;
        return 1;
    }

    // check given script file
    fs::path filePath(argv[1]);
    if(!exists(filePath)) {
       std::cerr << "error: script file does not exist: " << argv[1] << std::endl;
       return 2;
    }
    if(is_directory(filePath)) {
       std::cerr << "error: script file is a directory: " << argv[1] << std::endl;
       return 2;
    }
    fs::path parentPath = system_complete(filePath).parent_path();

    // initialize system
    system::System::setArguments(argc, argv);

    // create script host
    ScriptHost &host = ScriptHost::instance();
    host.setScriptFile(parentPath.c_str(), argv[1]);

    // add object caches
    ObjectCache *caches[] = {
                            &audio::AudioInputPortCache::instance(),
                            &audio::AudioOutputPortCache::instance(),
                            &audio::AudioStereoInputCache::instance(),
                            &audio::MixerCache::instance(),
                            &lv2::PluginCache::instance(),
                            &midi::MidiInputPortCache::instance(),
                            &midi::MidiOutputPortCache::instance(),
                            &transport::MasterCache::instance(),
                            &audio::BeatTrackerCache::instance(),
                            &midi::BeatTrackerCache::instance(),
                            &osc::InputFactory::instance(),
                            &osc::OutputFactory::instance(),
                            &audio::OnsetDetectorCache::instance()
                            };
    host.setObjectCaches(13, caches);

    // create and  start audioengine
    AudioEngine &audioEngine = AudioEngine::instance();
    int status = audioEngine.activate(argv[1]); // use script name as client name

    // exit if audio engine failed to start
    if(status) {
        std::cerr << "Failed to start audio engine: " << status << std::endl;
        return 1;
    }

    signal(SIGQUIT, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGHUP, signal_handler);
    signal(SIGINT, signal_handler);

    // sample rate now valid
    jack_nframes_t sampleRate = audioEngine.getSampleRate();
    lv2::PluginCache::instance().setSampleRate(sampleRate);

    // run script
    status = host.run();

    // script has ended
    ExtensionManager::instance().shutdown();
    osc::OutputFactory::instance().shutdown();
    audioEngine.shutdown();
    return status;
}
