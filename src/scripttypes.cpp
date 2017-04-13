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

#include "scripttypes.h"

#include <stdexcept>
#include <cstring>

namespace bipscript {

ScriptHashIterator::ScriptHashIterator(HSQUIRRELVM &vm, HSQOBJECT &hash)
    : vm(vm)
{
    sq_pushobject(vm, hash);
    sq_pushnull(vm); // iterator
}

bool ScriptHashIterator::hasNext()
{
    if(!SQ_SUCCEEDED(sq_next(vm,-2))){
        sq_pop(vm,1); // pop the iterator
        return false;
    }
    return true;
}

ScriptHashPair &ScriptHashIterator::next()
{
    pair.value.setValue(vm, -1);
    pair.key.setValue(vm, -2);
    sq_pop(vm,2); // pop value + key
    return pair;
}

void ScriptValue::setValue(HSQUIRRELVM &vm, SQInteger idx)
{
    switch(sq_gettype(vm,idx))
    {
        case OT_NULL:
            type = NULLVALUE;
            break;
        case OT_INTEGER:
            type = INTEGER;
            sq_getinteger(vm, idx, &intValue);
            break;
        case OT_FLOAT:
            type = FLOAT;
            sq_getfloat(vm, idx, &floatValue);
            break;
        case OT_STRING:
            type = STRING;
            sq_getstring(vm, idx, &stringValue);
            break;
        case OT_ARRAY:
            type = ARRAY;
            HSQOBJECT arrayObj;
            sq_getstackobj(vm, idx, &arrayObj);
            arrayValue = new ScriptArray(vm, arrayObj);
            break;
        case OT_TABLE:
            throw std::logic_error("Table not supported here");
        default:
            throw std::logic_error("Unknown type not supported here");
    }
}

void ScriptValue::pushValue(HSQUIRRELVM &vm)
{
    switch(type)
    {
        case BOOL:
            sq_pushbool(vm, intValue);
            break;
        case NULLVALUE:
            sq_pushnull(vm);
            break;
        case INTEGER:
            sq_pushinteger(vm, intValue);
            break;                    
        case FLOAT:
            sq_pushfloat(vm, floatValue);
            break;
        case STRING:
            sq_pushstring(vm, stringValue, std::strlen(stringValue));
            break;
        case ARRAY:
            throw std::logic_error("Array not supported here");
        case OT_TABLE:
            throw std::logic_error("Table not supported here");
        default:
            throw std::logic_error("Unknown type not supported here");
    }
}


ScriptArray::ScriptArray(HSQUIRRELVM &vm, HSQOBJECT &array) :
    vm(vm), array(array) {
    sq_pushobject(vm, array); // push array
    length = sq_getsize(vm, -1); // TODO: error handling
    sq_pop(vm, 1); // pop array
}

ScriptValue &ScriptArray::operator[](const int index)
{
    sq_pushobject(vm, array);
    sq_pushinteger(vm, index);
    sq_get(vm, -2);
    element.setValue(vm, -1);
    sq_pop(vm, 2); // pop element + array
    return element;
}

bool ScriptFunctionClosure::execute(HSQOBJECT &context)
{
    // push function pointer as closure
    sq_pushobject(vm, function);
    // push context as "this" pointer
    sq_pushobject(vm, context);
    // sublclasses add optional parameters
    addParameters();
    // attempt to call function
    bool success = true;
    if(SQ_FAILED(sq_call(vm, numargs, SQFalse, SQTrue))) {
        success = false;
    }
    // pop closure
    sq_pop(vm, 1);
    return success;
}

}