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

class OscMessage
{
    std::string path;
public:
    OscMessage(std::string path)
        : path(path) {}
    const char *getPath() {
        return path.c_str();
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
