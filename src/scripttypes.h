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

enum ScriptValueType {
    NULLVALUE, INTEGER, FLOAT, STRING
};

struct ScriptValue
{
    ScriptValueType type;
    union {
        SQInteger intValue;
        SQFloat floatValue;
        const SQChar *stringValue;
    };
    void setValue(HSQUIRRELVM &vm, SQInteger idx);
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

#endif // SCRIPTTYPES_H
