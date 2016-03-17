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

#include "drumtabreader.h"
#include <string.h>

#include <stdio.h>
#include <ctype.h>
#include <string>
#include <map>

using namespace std;

Pattern *pattern; // make it a member

map<string, int> noteValue;
map<string, int> noteVelocity;

void init() {
    noteValue["SDo"] = 38;
    noteValue["SNo"] = 38;
    noteValue["SNO"] = 38;
    noteValue["S o"] = 38;
    noteValue["S g"] = 38;
    noteValue["SNg"] = 38;
    noteValue["S d"] = 38;
    noteValue["SNd"] = 38;
    noteValue["SDf"] = 40; // flam
    noteValue["SNF"] = 40; // flam
    noteValue["S F"] = 40; // flam

    noteValue["BDo"] = 36;
    noteValue["B o"] = 36;

    noteValue["RDX"] = 51;
    noteValue["CRx"] = 51; // crash-ride?
    noteValue["CCX"] = 49;
    noteValue["C1X"] = 49;
    noteValue["C2X"] = 57;

    noteValue["HHx"] = 42; // hihat closed
    noteValue["HHX"] = 42;
    noteValue["HHo"] = 42;
    noteValue["HHg"] = 42;
    noteValue["HFx"] = 44; // hihat pedal
    noteValue["HHO"] = 46; // hihat open

    noteValue["T1o"] = 48; // tom 1
    noteValue["T2o"] = 47; // tom 2
    noteValue["T3o"] = 43; // tom 3

    noteValue["FTo"] = 45; // floor tom
    noteValue["FTO"] = 45; // floor tom
    noteValue["F1o"] = 45; // floor tom 1
    noteValue["F2o"] = 45; // floor tom 2

    // ---- velocities

    noteVelocity["SDo"] = 111;
    noteVelocity["SNO"] = 127;
    noteVelocity["S d"] = 22; // drag
    noteVelocity["SNd"] = 22; // drag
    noteVelocity["S g"] = 33;

    noteVelocity["BDo"] = 111;

    noteVelocity["HHX"] = 127;
    noteVelocity["HHg"] = 33;

    noteVelocity["FTO"] = 127; // floor tom
}


void processBar(int bar, const char *channel, const char* line, int len) {
    //printf("got bar for %.*s, length %d: %.*s\n", 2, channel, len, len, bar);
    for(int i = 0; i < len; i++) {
        if(line[i] != '-') {
            int notenum, velocity;
            string hit;
            hit += toupper(channel[0]);
            hit += toupper(channel[1]);
            if(isdigit(channel[0]) && isdigit(channel[1])) {
                notenum = atoi(hit.c_str());
                velocity = 127; // TODO: look at actual hit notation
            }
            else {
                hit += line[i];
                notenum = noteValue[hit];
                if(notenum == 0) {
                    printf("!!notevalue for %s is %d\n", hit.c_str(), notenum);
                }
                velocity = noteVelocity[hit];
                if(velocity == 0) {
                    velocity = 127; // default
                }
            }
            //printf(" adding %s (=%d/%d) at %d:%d/%d\n", hit.c_str(), notenum, velocity, bar, i, len);
            Note note(notenum, velocity, 1, len);
            pattern->addNote(note, bar, i, len);
        }
    }
}

int processTabLine(int startBar, const char *line, int len) {
    // grab the channel
    const char *channel = line;
    line += 3;
    // identify bars
    int last = 0;
    int barCounter = startBar;
    for(int i = 0; i < len - 3; i++) {
        if(line[i] == '|') {
            processBar(barCounter++, channel, &line[last], i - last);
            last = i + 1;
        }
    }
    return barCounter - startBar;
}

void DrumTabReader::define(string code, string hit, int note, int velocity) {
    string key = code + hit;
    noteValue[key] = note;
    noteVelocity[key] = velocity;
}

Pattern* DrumTabReader::read(const char*tab)
{
    init();
    pattern = new Pattern();
    int last = 0;
    int lineBars = 0;
    int startBar = 1;
    size_t tablen = strlen(tab);
    for(unsigned int i = 0; i <= tablen; i++) {
        if(i != tablen && tab[i] != '\n') {
            continue;
        }
        // pointer to line
        const char* line = &tab[last];
        int len = i - last;
        // trim leading space
        while(isspace(*line) && len > 0) {
            line++;
            len--;
        }
        // determine line type
        if(len > 3 && line[2] == '|') {
            lineBars = processTabLine(startBar, line, len);
        } else if(len == 0) {
            startBar += lineBars;
            lineBars = 0;
        } else {
            printf("ignoring line: %.*s\n", len, line);
        }
        last = i;
    }
    return pattern;
}

