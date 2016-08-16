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
#include "transport.h"
#include <iostream>

void Transport::schedule(ScriptFunction &function, unsigned int bar, unsigned int position, unsigned int division)
{
    eventBuffer.addEvent(new AsyncClosure(function, bar, position, division));
}

void Transport::process(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time)
{
    AsyncClosure *closure = static_cast<AsyncClosure*>(eventBuffer.getNextEvent(rolling, pos, nframes));
    while(closure) {
        closure->dispatch();
        closure = static_cast<AsyncClosure*>(eventBuffer.getNextEvent(rolling, pos, nframes));
    }
}
