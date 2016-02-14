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

enum ScriptValueType {
    NULLVALUE, INTEGER, FLOAT, STRING, ARRAY
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
    HSQOBJECT function;
    uint32_t numargs;
public:
    ScriptFunction(HSQOBJECT &func, uint32_t numargs) :
        function(func), numargs(numargs) {}
    HSQOBJECT &getFunction() {
        return function;
    }
    uint32_t getNumargs() {
        return numargs;
    }
};

#endif // SCRIPTTYPES_H
