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
#ifndef SCRIPTHOST_H
#define SCRIPTHOST_H

#include <list>
#include <atomic>
#include "squirrel.h"

#include "position.h"
#include "objectcache.h"

class AsyncFunction : public Position {
    HSQOBJECT functionPtr;
public:
    AsyncFunction(HSQOBJECT &functionPtr, unsigned int bar,
                  unsigned int position, unsigned int division) :
        Position(bar, position, division), functionPtr(functionPtr) {}
    HSQOBJECT &getFunction() {
        return functionPtr;
    }
};

class ScriptHost
{
    HSQUIRRELVM vm;
    std::string folder;
    const char *filename;

    // async methods
    std::list<AsyncFunction> asyncFunctions;

    // object caches
    uint16_t objectCacheCount;
    ObjectCache **objectCacheList;

    // script thread communication
    std::atomic<bool> abort;
    std::atomic<bool> restart;
    std::atomic<bool> running;

    // singleton
    ScriptHost() : abort(false), restart(false), running(true)  {}
    ScriptHost(ScriptHost const&) = delete;
    void operator=(ScriptHost const&);
public:
    static ScriptHost& instance() {
        static ScriptHost instance;
        return instance;
    }
    void setScriptFile(const char *folder, const char *filename) {
        this->folder = folder;
        this->filename = filename;
    }
    void setObjectCaches(uint16_t count, ObjectCache *list[]) {
        objectCacheCount = count;
        objectCacheList = list;
    }
    std::string &getCurrentFolder() {
        return folder;
    }
    int run();
    void schedule(HSQOBJECT &function, unsigned int bar, unsigned int position, unsigned int division);
    // for AE
    void process(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time);
    bool reposition(uint16_t attempt);

private:
    void objectReposition(bool final);
    void bindModules(HSQUIRRELVM vm);
    bool waitUntil(Position &pos);
    bool waitForRestart();
};

#endif // SCRIPTHOST_H

