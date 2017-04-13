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
#ifndef SCRIPTTYPES_H
#define SCRIPTTYPES_H

#include "squirrel.h"
#include <stdint.h>
#include "methodqueue.h"

namespace bipscript {

enum ScriptValueType {
    NULLVALUE, BOOL, INTEGER, FLOAT, STRING, ARRAY
};

class ScriptArray;

struct ScriptValue
{
    ScriptValueType type;
    union {
        SQInteger intValue;
        SQFloat floatValue;
        const SQChar *stringValue;
        ScriptArray *arrayValue;
    };
    void setValue(HSQUIRRELVM &vm, SQInteger idx);
    void pushValue(HSQUIRRELVM &vm);
};

class ScriptArray
{
    HSQUIRRELVM &vm;
    HSQOBJECT array;
    ScriptValue element;
    uint32_t length;
public:
    ScriptArray(HSQUIRRELVM &vm, HSQOBJECT &array);
    uint32_t size() const {
        return length;
    }
    ScriptValue &operator[](const int index);
};

struct ScriptHashPair
{
    ScriptValue key;
    ScriptValue value;
};

class ScriptHashIterator
{
    HSQUIRRELVM &vm;
    ScriptHashPair pair;
public:
    ScriptHashIterator(HSQUIRRELVM &vm, HSQOBJECT &hash);
    bool hasNext();
    ScriptHashPair &next();
};

class ScriptFunction
{
protected:
    HSQOBJECT function;
    uint32_t numargs;
    HSQUIRRELVM vm;
public:
    ScriptFunction(HSQUIRRELVM &vm, HSQOBJECT &func, uint32_t numargs) :
        vm(vm), function(func), numargs(numargs) {}
    HSQOBJECT &getFunction() {
        return function;
    }
    uint32_t getNumargs() {
        return numargs;
    }
    void release() {
        sq_release(vm, &function);
    }
};

class ScriptFunctionClosure : public ScriptFunction
{
public:
    ScriptFunctionClosure(ScriptFunction &function) :
        ScriptFunction(function) {}
    bool execute(HSQOBJECT &context);
    void dispatch() {
        MethodQueue::instance().dispatch(this);
    }
    virtual ~ScriptFunctionClosure() {}
    virtual void recycle() = 0;
protected:
    void addFloat(float f) {
        sq_pushfloat(vm, f);
    }
    void addInteger(int i) {
        sq_pushinteger(vm, i);
    }
    void addString(const char *s, int size) {
        sq_pushstring(vm, s, size);
    }
    void addObject(void *obj, HSQOBJECT &type) {
        sq_pushobject(vm, type);
        sq_createinstance(vm, -1);
        sq_remove(vm, -2);
        sq_setinstanceup(vm, -1, obj);
    }
    virtual void addParameters() = 0;
};

}

#endif // SCRIPTTYPES_H
