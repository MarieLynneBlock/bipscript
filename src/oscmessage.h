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
#ifndef OSCMESSAGE_H
#define OSCMESSAGE_H

#include "event.h"
#include "scripttypes.h"

#include <string>
#include <cstring>

namespace bipscript {
namespace osc {

struct OscParameter
{
    char type;
    union {
        int intValue;
        float floatValue;
        const char *stringValue;
    } value;
    OscParameter(char type) : type(type) {}
};

class OscMessage
{
    std::string path;
    std::vector<OscParameter> parameters;
public:
    OscMessage(std::string path)
        : path(path) {}
    OscMessage(OscMessage &copy) :
      path(copy.path), parameters(copy.parameters)
    {
        for (auto & param: parameters) {
            if(param.type == 's') {
                size_t len = std::strlen(param.value.stringValue) + 1;
                char* const copy = new char[len];
                std::strncpy(copy, param.value.stringValue, len);
                param.value.stringValue = copy;
            }
        }
    }
    ~OscMessage() {
        for (auto & param: parameters) {
            if(param.type == 's') {
                delete[] param.value.stringValue;
            }
        }
    }
    const char *getPath() { return path.c_str(); }
    void addInteger(int value) {
        OscParameter param('i');
        param.value.intValue = value;
        parameters.push_back(param);
    }
    void addFloat(float value) {
        OscParameter param('f');
        param.value.floatValue = value;
        parameters.push_back(param);
    }
    void addString(const char *value) {
        OscParameter param('s');
        size_t len = std::strlen(value) + 1;
        char* const copy = new char[len];
        std::strncpy(copy, value, len);
        param.value.stringValue = copy;
        parameters.push_back(param);
    }
    void addBoolean(bool value) {
        OscParameter param(value ? 'T' : 'F');
        parameters.push_back(param);
    }
    void addNull(const char*) {
        OscParameter param('N');
        parameters.push_back(param);
    }
    int getParameterCount() {
        return parameters.size();
    }
    OscParameter getParameter(int i) {
        return parameters[i];
    }
    ScriptValue arg(int i) {
      ScriptValue value;
      switch(parameters[i].type) {
      case 'i':
        value.type = INTEGER;
        value.intValue = parameters[i].value.intValue;
        break;
      case 'f':
        value.type = FLOAT;
        value.floatValue = parameters[i].value.floatValue;
        break;
      case 's':
        value.type = STRING;
        value.stringValue = parameters[i].value.stringValue;
        break;
      case 'T':
        value.type = BOOL;
        value.intValue = 1;
        break;
      case 'F':
        value.type = BOOL;
        value.intValue = 0;
        break;
      case 'N':
        value.type = NULLVALUE;
        break;
      }
      return value;
    }
};

class OscEvent : public Event
{
    OscMessage message;
public:
    OscEvent(Position &pos, OscMessage &message)
        : Event(pos), message(message) {}
    OscMessage &getMessage() {
        return message;
    }
};

}}

#endif // OSCMESSAGE_H
