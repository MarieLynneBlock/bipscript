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
#ifndef DRUMTABPARSER_H
#define DRUMTABPARSER_H

#include "midipattern.h"
#include <string>
#include <map>


class DrumTabReader
{
    std::map<std::string, int> noteValue;
    std::map<std::string, int> noteVelocity;
    Pattern *pattern;
public:
    DrumTabReader();
    Pattern *read(const char*);
    void setVelocity(int note, const char *hit, int velocity) {
        define(note, hit, velocity);
    }
private:
    int processTabLine(int startBar, const char *line, int len);
    void processBar(int bar, const char *channel, const char *line, int len);
    void define(std::string code, std::string hit, int note, int velocity);
    void define(int note, std::string hit, int velocity);
};

#endif // DRUMTABPARSER_H
