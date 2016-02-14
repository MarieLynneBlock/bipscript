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
#ifndef TIMEPACKAGE_H
#define TIMEPACKAGE_H

#include "position.h"
#include "scripthost.h"
#include "scripttypes.h"

class Time
{
public:
    //static Position &now();
    static void schedule(ScriptFunction &function, unsigned int bar) {
        schedule(function, bar, 1);
    }
    static void schedule(ScriptFunction &function, unsigned int bar, unsigned int position) {
        schedule(function, bar, position, 4); // TODO: base it on time signature
    }
    static void schedule(ScriptFunction &function, unsigned int bar, unsigned int position, unsigned int division) {
        ScriptHost::instance().schedule(function, bar, position, division);
    }
};

#endif // TIMEPACKAGE_H
