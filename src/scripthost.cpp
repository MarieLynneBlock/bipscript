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
#include <stdarg.h>

#include "squirrel.h"
#include "sqstdaux.h"
#include "sqstdmath.h"
#include "sqstdio.h"

#include "bindings.h"

#include "scripthost.h"
#include "audioengine.h"
#include "objectcollector.h"
#include <iostream>

void ScriptHost::process(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time)
{
    for(uint16_t i = 0; i < objectCacheCount; i++) {
        objectCacheList[i]->process(rolling, pos, nframes, time);
    }
}

void ScriptHost::objectReposition(bool final)
{
    for(uint16_t i = 0; i < objectCacheCount; i++) {
        objectCacheList[i]->reposition();
    }
}

// from the API docs: TRUE (non-zero) when ready to roll
bool ScriptHost::reposition(uint16_t attempt)
{
    if(running.load()) {
        if(!attempt) {
            // notify script thread
            abort.store(true);
        }
        this->objectReposition(false); // kill some time
    }

    // check that the script is ready
    if(running.load()) {
        return false; // it's not
    }

    this->objectReposition(true); // final object reposition
    abort.store(false);
    restart.store(true);

    // ready to roll
    return true;
}

static void squirrel_print_function(HSQUIRRELVM sqvm, const SQChar *format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
}

void ScriptHost::schedule(HSQOBJECT &function, unsigned int bar, unsigned int position, unsigned int division)
{
    this->asyncFunctions.push_back(AsyncFunction(function, bar, position, division));
}

bool ScriptHost::waitUntil(Position &pos)
{
    jack_position_t jack_pos;
    AudioEngine &ae = AudioEngine::instance();
    bool rolling(ae.getPosition(jack_pos));
    // how many frames = 1/20th of a second
    jack_nframes_t gap = ae.getSampleRate() / 20;
    while(!rolling || pos.calculateFrameOffset(jack_pos) > gap) {
        // first check we don't have to restart
        if(abort.load()) {
            return false;
        }
        // free collected objects
        ObjectCollector::instance()->free();
        // sleep
        struct timespec req = {0, 25000};
        while(nanosleep(&req,&req)==-1) {
            if(abort.load()) { return false; }
            continue;
        }
        rolling = (ae.getPosition(jack_pos));
    }
    // std::cout << " -- wait is over, frames until " << pos << " is " << pos.calculateFrameOffset(jack_pos) << std::endl;
    return true;
}


bool ScriptHost::waitForRestart()
{
    running.store(false);
    bool activeObjects = false;
    for(uint16_t i = 0; i < objectCacheCount; i++) {
        if(objectCacheList[i]->scriptComplete()) {
            activeObjects = true;
        }
    }
    if(!activeObjects) {
        return false;
    }
    while(true) {
        if(restart.load()) {
            restart.store(false);
            running.store(true);
            return true;
        }
        // free collected objects
        ObjectCollector::instance()->free();
        // sleep
        struct timespec req = {0, 25000};
        while(nanosleep(&req,&req)==-1) {
            continue;
        }
    }
    return true;
}

int ScriptHost::run() {
    // init squirrel
    vm = sq_open(1024);
    // set print function
    sqstd_seterrorhandlers(vm);
    sq_setprintfunc(vm, squirrel_print_function, squirrel_print_function);
    // standard library modules
    sq_pushroottable(vm);
    sqstd_register_mathlib(vm);
    sqstd_register_iolib(vm);
    // add local modules to squirrel
    // bindModules(vm);
    binding::bindAudio(vm);
    binding::bindMidi(vm);
    binding::bindLv2(vm);    
    binding::bindIO(vm);
    binding::bindTime(vm);
    binding::bindMath(vm);
    // pop root table
    sq_pop(vm, 1);

    bool rerun = true;
    while(rerun) {
        // fresh run table
        HSQOBJECT freshRunTable;
        sq_newtable(vm);
        sq_getstackobj(vm,-1,&freshRunTable);
        sq_addref(vm, &freshRunTable);
        if(!SQ_SUCCEEDED(sqstd_dofile(vm, filename, 0, SQTrue))) {
            const SQChar *error;
            sq_getlasterror(vm);
            if (SQ_SUCCEEDED(sq_getstring(vm, -1, &error))) {
                std::cerr << error << std::endl;
            }
            return 1;
        }
        // pop fresh run table
        sq_pop(vm, 1);

        // now do the async methods
        asyncFunctions.sort();
        for(std::list<AsyncFunction>::iterator it = asyncFunctions.begin(); it != asyncFunctions.end(); ++it) {
            // push function pointer as closure
            sq_pushobject(vm, it->getFunction());
            // how many params does this function expect?
            SQUnsignedInteger nparams, nfreevars;
            sq_getclosureinfo(vm, -1, &nparams, &nfreevars);
            // push fresh run table as "this" pointer
            sq_pushobject(vm, freshRunTable);
            // push remaining arguments
            if(nparams > 1) {
                sq_pushinteger(vm, it->getBar());
            }
            if(nparams > 2) {
                sq_pushinteger(vm, it->getPosition());
            }
            if(nparams > 3) {
                sq_pushinteger(vm, it->getDivision());
            }
            // now sleep until time
            //Position pos(it->getBar(), it->getPosition(), 4);
            // TODO: break
            if(!waitUntil(*it)) {
                break; // TODO cleanup
            }
            // attempt to call function
            if(SQ_FAILED(sq_call(vm, nparams, SQFalse, SQTrue))) {
                // release and pop closure
                sq_release(vm, &it->getFunction());
                sq_pop(vm, 1);
                // break loop
                return 1; // TODO: still need to release the remaining closures?
            }
            // release and pop closure
            sq_release(vm, &it->getFunction());
            sq_pop(vm, 1);
        }
        // clear list
        asyncFunctions.clear();
        // release fresh run table
        sq_release(vm, &freshRunTable);

        rerun = waitForRestart();
    }
    // shut down squirrel
    sq_close(vm);
    return 0;
}
