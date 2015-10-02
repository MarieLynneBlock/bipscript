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
#ifndef SOURCE_H
#define SOURCE_H

#include <jack/types.h>

class Source {
public:
    virtual bool connectsTo(Source *) = 0;
    virtual void process(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time) = 0;
};

#endif // SOURCE_H
