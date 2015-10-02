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
#ifndef STEREO_H
#define STEREO_H

#include <string>
#include "audioport.h"

class StereoOutput
{
    AudioOutputPort *portLeft;
    AudioOutputPort *portRight;
public:
    void connect(AudioSource &source);
    StereoOutput(std::string name, const char *connectLeft, const char *connectRight);
};

#endif // STEREO_H
