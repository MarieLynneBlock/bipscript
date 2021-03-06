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
#ifndef RANDOM_H
#define RANDOM_H

#include <random>
#include <time.h>
#include <stdexcept>

namespace bipscript {
namespace math {

class Random
{
public:
    Random() {
        srand(time(NULL));
    }
    int integer(unsigned int max) {
        if(max < 1) {
            throw std::logic_error("max cannot be less than one");
        }
        return rand() % max;
    }
};

}}

#endif // RANDOM_H
