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

#include <string>

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
        param.value.stringValue = value;
        parameters.push_back(param);
    }
    void addBoolean(bool value) {
        OscParameter param(value ? 'T' : 'F');
        parameters.push_back(param);
    }
    int getParameterCount() {
        return parameters.size();
    }
    OscParameter getParameter(int i) {
        return parameters[i];
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

#endif // OSCMESSAGE_H
