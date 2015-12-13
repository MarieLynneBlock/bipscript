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
#include "midipattern.h"

#include <sstream>

void Pattern::addNote(Note &note, int bar, int position, int division)
{
    Position start(bar, position, division);
    // std::cout << "adding note with duration " << note.duration << " to pattern at " << start << std::endl;
    noteList.push_back(PatternNote(note, start));
}

void Pattern::addNote(Note &note, Position &position) {
    // std::cout << "adding note with duration " << note.duration << " to pattern at " << position << std::endl;
    noteList.push_back(PatternNote(note, position));
}

std::string Pattern::print() {
    std::stringstream sstream;
    sstream << noteList.size() << " events" << std::endl;
    for(auto it = noteList.begin(); it != noteList.end(); it++) {
        sstream << "N" << it->note.note;
        sstream << " duration " << it->note.duration;
        sstream << " " << it->position;
        sstream << std::endl;
   }
   return sstream.str();
}
