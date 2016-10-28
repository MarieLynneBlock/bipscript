#ifndef OBJECTCACHE_H
#define OBJECTCACHE_H
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

#include <map>
#include <set>
#include <iostream>

#include "audioengine.h"

class ObjectCache
{
public:
    /**
     * Called after the script has completed so factories can reset for the next run.
     *
     * Returns true if there are active objects and the application should stay alive.
     *
     * Runs in the script thread.
     */
    virtual bool scriptComplete() = 0;
};

/**
 * Cache template for resuable objects
 *
 * T must be a Listable
 */
template <class T> class ActiveCache : public ObjectCache
{
    std::map<int, T*> instanceMap;
    std::set<T*> activeScriptObjects;
    virtual void scriptReset() {}
    virtual void newObject(T *) = 0;
    virtual void removeObject(T *) = 0;
protected:
    /**
     * find an existing object with this key
     */
    T *findObject(int key) {
        T *ret = instanceMap[key];
        if(ret) {
            activeScriptObjects.insert(ret);
        }
        return ret;
    }
    /**
     * add a new object with key
     */
    void registerObject(int key, T *obj) {
        instanceMap[key] = obj;
        activeScriptObjects.insert(obj);
        newObject(obj);
    }
    /**
      * remove all existing objects
      */
    void removeAll() {
        for(auto iterator = instanceMap.begin(); iterator != instanceMap.end();) {
            T *obj = iterator->second;
            auto it = activeScriptObjects.find(obj);
            if(it != activeScriptObjects.end()) {
                activeScriptObjects.erase(it);
            }
            iterator = instanceMap.erase(iterator);
            removeObject(obj);
        }
    }
public:
    /**
      * Restore cache state after script has completed.
      *
      * Runs in script thread.
      *
      */
    bool scriptComplete() {
        bool activeObjects = false;
        // loop over cached ports
        for(auto iterator = instanceMap.begin(); iterator != instanceMap.end();) {
            T *obj = iterator->second;
            // object was used in the last script run?
            auto it = activeScriptObjects.find(obj);
            if(it != activeScriptObjects.end()) {
                // std::cout << typeid(T).name() << " was used in the last run: " << iterator->first << std::endl;
                activeScriptObjects.erase(it);
                activeObjects = true;
                iterator++;
            } else {
                // remove object from map and audio engine
                // std::cout << typeid(T).name() << " NOT used in the last run: " << iterator->first << std::endl;
                iterator = instanceMap.erase(iterator);
                removeObject(obj);
            }
        }
        scriptReset();
        return activeObjects;
    }
};

/**
 * Cache template for resuable processors
 *
 * T must be a Processor
 */
template <class T> class ProcessorCache : public ActiveCache<T>
{
    void newObject(T *obj) {
        AudioEngine::instance().addProcessor(obj);
    }

    void removeObject(T *obj) {
        AudioEngine::instance().removeProcessor(obj);
    }
};

#endif // OBJECTCACHE_H
