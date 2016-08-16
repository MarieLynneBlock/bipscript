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
#include "sqstdstring.h"

#include "bindings.h"

#include "scripthost.h"
#include "extension.h"
#include "audioengine.h"
#include "objectcollector.h"
#include <iostream>

void ScriptHost::process(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time)
{
    for(uint16_t i = 0; i < objectCacheCount; i++) {
        objectCacheList[i]->process(rolling, pos, nframes, time);
    }
}

// from the API docs: TRUE (non-zero) when ready to roll
bool ScriptHost::reposition(uint16_t attempt)
{
    if(!attempt) { // first run- notify
        if(running.load()) {
            abort.store(true);
        }
        for(uint16_t i = 0; i < objectCacheCount; i++) {
            objectCacheList[i]->reposition();
        }
    }
    // check that the script is ready
    if(running.load()) {
        return false; // it's not
    }
    // check script objects are ready
    for(uint16_t i = 0; i < objectCacheCount; i++) {
        if(!objectCacheList[i]->repositionComplete()) {
            return false;
        }
    }
    // ready to roll
    abort.store(false);
    restart.store(true);
    return true;
}

static void squirrel_print_function(HSQUIRRELVM sqvm, const SQChar *format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
}

bool ScriptHost::waitForRestart(HSQOBJECT &context)
{
    running.store(false);
    bool activeObjects = false;
    for(uint16_t i = 0; i < objectCacheCount; i++) {
        if(objectCacheList[i]->scriptComplete()) {
            activeObjects = true;
        }
    }
    if(!activeObjects) {
        activeObjects = ExtensionManager::instance().scriptComplete();
    }
    if(!activeObjects) {
        return false;
    }
    while(true) {
        if(restart.load()) {
            // release fresh run table
            sq_release(vm, &context);
            restart.store(false);
            running.store(true);
            return true;
        }
        // run any dispatched methods
        ScriptFunctionClosure *closure = MethodQueue::instance().next();
        while(closure) {
            closure->execute(context);
            delete closure;
            closure = MethodQueue::instance().next();
        }
        // free collected objects
        ObjectCollector::instance()->free();
        // sleep
        struct timespec req = {0, 25000};
        while(nanosleep(&req,&req)==-1) {
            continue;
        }
    }
}

int ScriptHost::run() {
    // init squirrel
    vm = sq_open(1024);
    // set print function
    sqstd_seterrorhandlers(vm);
    sq_setprintfunc(vm, squirrel_print_function, squirrel_print_function);
    // standard library modules
    sq_pushroottable(vm);
    // load extensions first
    try {
        ExtensionManager::instance().bindAll(vm);
    }
    catch(std::logic_error e) {
        std::cerr << "warning: extension loading failed: " << e.what() << std::endl;
    }
    // standard library modules
    sqstd_register_mathlib(vm);
    sqstd_register_iolib(vm);
    sqstd_register_stringlib(vm);
    // add local modules to squirrel
    // bindModules(vm);
    binding::bindAudio(vm);
    binding::bindMidi(vm);
    binding::bindLv2(vm);    
    binding::bindIO(vm);
    binding::bindOsc(vm);
    binding::bindTime(vm);
    binding::bindTransport(vm);
    binding::bindMath(vm);
    binding::bindSystem(vm);
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

        // wait for restart
        rerun = waitForRestart(freshRunTable);
    }
    // shut down extensions
    ExtensionManager::instance().shutdown();
    // shut down squirrel
    sq_close(vm);
    return 0;
}
