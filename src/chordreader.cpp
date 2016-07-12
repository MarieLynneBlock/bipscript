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
#include "chordreader.h"
// #include <cstring>

ChordReader::ChordReader()
{
    // major
    static int int_maj[3] = {4, 7, 0};
    intervalMap[""] = int_maj;
    intervalMap["maj"] = int_maj;
    // minor
    static int int_minor[3] = {3, 7, 0};
    intervalMap["m"] = int_minor;
    intervalMap["min"] = int_minor;
    // dom7
    static int int_7[4] = {4, 7, 10, 0};
    intervalMap["7"] = int_7;
    // min dom7
    static int int_m7[4] = {3, 7, 10, 0};
    intervalMap["m7"] = int_m7;
    // min 7 flat 5
    static int int_m7b5[4] = {3, 6, 10, 0};
    intervalMap["m7b5"] = int_m7b5;
    // major 7
    static int int_maj7[4] = {4, 7, 11, 0};
    intervalMap["maj7"] = int_maj7;
    intervalMap["M7"] = int_maj7;
    // 6
    static int int_6[4] = {4, 7, 9, 0};
    intervalMap["6"] = int_6;
    // m6
    static int int_m6[4] = {3, 7, 9, 0};
    intervalMap["m6"] = int_m6;
    // aug
    static int int_aug[3] = {4, 8, 0};
    intervalMap["aug"] = int_aug;
    // plus
    static int int_plus[3] = {4, 8, 0};
    intervalMap["plus"] = int_plus;
    // aug7
    static int int_aug7[4] = {4, 8, 10, 0};
    intervalMap["aug7"] = int_aug7;
    // dim
    static int int_dim[3] = {3, 6, 0};
    intervalMap["dim"] = int_dim;
    // dim7
    static int int_dim7[4] = {3, 6, 9, 0};
    intervalMap["dim7"] = int_dim7;
    // 9
    static int int_9[5] = {4, 7, 10, 14, 0};
    intervalMap["9"] = int_9;
    // m9
    static int int_m9[5] = {3, 7, 10, 14, 0};
    intervalMap["m9"] = int_m9;
    // maj9
    static int int_maj9[5] = {4, 7, 11, 14, 0};
    intervalMap["maj9"] = int_maj9;
    // M9
    static int int_M9[5] = {4, 7, 11, 14, 0};
    intervalMap["M9"] = int_M9;
    // 11
    static int int_11[6] = {4, 7, 10, 14, 17, 0};
    intervalMap["11"] = int_11;
    // dim9
    static int int_dim9[5] = {3, 6, 9, 13, 0};
    intervalMap["dim9"] = int_dim9;
    // sus
    static int int_sus[3] = {5, 7, 0};
    intervalMap["sus"] = int_sus;
    // sus4
    static int int_sus4[3] = {4, 7, 0};
    intervalMap["sus4"] = int_sus4;
    // sus9
    static int int_sus9[3] = {2, 7, 0};
    intervalMap["sus9"] = int_sus9;
    // 7sus4
    static int int_7sus4[4] = {5, 7, 10, 0};
    intervalMap["7sus4"] = int_7sus4;
    // 7sus9
    static int int_7sus9[4] = {2, 7, 10, 0};
    intervalMap["7sus9"] = int_7sus9;
    // 5th
    static int int_5[2] = {7, 0};
    intervalMap["5"] = int_5;
}

Pattern *ChordReader::read(const char *abc, int duration, int division) {
    int stringLen = strlen(abc);
    if(stringLen < 1) {
        throw std::logic_error("Chord cannot be an empty string");
    }

    // check first character
    int letter = toupper(abc[0]);
    if(letter < 65 || letter > 71) {
        throw std::logic_error("Chord must start with A-G");
    }
    const char *chordType = abc + 1;

    // translate ASCII to MIDI
    int pitch;
    if(letter == 'A') {
        pitch = 57;
    } else if(letter == 'B') {
        pitch = 59;
    } else if(letter == 'C') {
        pitch = 60;
    } else if(letter == 'D') {
        pitch = 62;
    } else if(letter == 'E') {
        pitch = 64;
    } else if(letter == 'F') {
        pitch = 65;
    } else if(letter == 'G') {
        pitch = 67;
    }
    // check for accidentals
    if(stringLen > 1 && abc[2] == 'b') {
        pitch--;
        chordType++;
    } else if(stringLen > 1 && abc[2] == '#') {
        pitch++;
        chordType++;
    }

    // look up chord in map
    auto it = intervalMap.find(chordType);
    if(it == intervalMap.end()) {
        std::string mesg("Unknown chord type: ");
        mesg.append(chordType);
        throw std::logic_error(mesg);
    }
    int *interval = it->second;

    // start pattern and add root
    Pattern *ret = new Pattern();
    Position position;
    Note note(pitch, 127, duration, division);
    ret->addNote(note, position);

    // add intervals
    int i = 0;
    while(interval[i] != 0) {
        Note note(pitch + interval[i], 127, duration, division);
        ret->addNote(note, position);
        i++;
    }
    return ret;
}
