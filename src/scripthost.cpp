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
#include "objectcollector.h"
#include <iostream>

namespace bipscript {

static void squirrel_print_function(HSQUIRRELVM sqvm, const SQChar *format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
}

bool ScriptHost::waitForRestart(HSQOBJECT &context)
{
    runningFlag.store(false);
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
        if(restartFlag.load()) {
            // release fresh run table
            sq_release(vm, &context);
            restartFlag.store(false);
            runningFlag.store(true);
            return true;
        }
        // run any dispatched methods
        ScriptFunctionClosure *closure = MethodQueue::instance().next();
        while(closure) {
            closure->execute(context);
            closure->recycle();
            closure = MethodQueue::instance().next();
        }
        // free collected objects
        ObjectCollector::scriptCollector().free();
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
    binding::bindAll(vm);
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

}