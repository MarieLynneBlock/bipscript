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
#ifndef MIDIPACKAGE_H
#define MIDIPACKAGE_H

#include "abcreader.h"

namespace bipscript {
namespace midi {

class Pattern;

class Midi
{
    static ABCReader& abcReader() {
        static ABCReader instance;
        return instance;
    }
public:
    static Pattern *abc(const char *str, const char *key, const char *noteLength,
                        const char *meter) {
        return abcReader().read(str, key, noteLength, meter);
    }
    static Pattern *abc(const char *str, const char *key, const char *noteLength) {
        return abcReader().read(str, key, noteLength);
    }
    static Pattern *abc(const char *str, const char *key) {
        return abcReader().read(str, key);
    }
    static Pattern *abc(const char *str) {
        return abcReader().read(str);
    }
};

}}

#endif // MIDIPACKAGE_H
