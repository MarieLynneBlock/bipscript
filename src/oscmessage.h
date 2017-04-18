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

struct Parameter
{
    char type;
    union {
        int intValue;
        float floatValue;
        const char *stringValue;
    } value;
    Parameter(char type) : type(type) {}
};

class Message
{
    std::string path;
    std::vector<Parameter> parameters;
public:
    Message(std::string path)
        : path(path) {}
    Message(Message &copy) :
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
    ~Message() {
        for (auto & param: parameters) {
            if(param.type == 's') {
                delete[] param.value.stringValue;
            }
        }
    }
    const char *getPath() { return path.c_str(); }
    void addInteger(int value) {
        Parameter param('i');
        param.value.intValue = value;
        parameters.push_back(param);
    }
    void addFloat(float value) {
        Parameter param('f');
        param.value.floatValue = value;
        parameters.push_back(param);
    }
    void addString(const char *value) {
        Parameter param('s');
        size_t len = std::strlen(value) + 1;
        char* const copy = new char[len];
        std::strncpy(copy, value, len);
        param.value.stringValue = copy;
        parameters.push_back(param);
    }
    void addBoolean(bool value) {
        Parameter param(value ? 'T' : 'F');
        parameters.push_back(param);
    }
    void addNull(const char*) {
        Parameter param('N');
        parameters.push_back(param);
    }
    int getParameterCount() {
        return parameters.size();
    }
    Parameter getParameter(int i) {
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

class Event : public bipscript::Event
{
    Message message;
public:
    Event(Position &pos, Message &message)
        : bipscript::Event(pos), message(message) {}
    Message &getMessage() {
        return message;
    }
};

}}

#endif // OSCMESSAGE_H
