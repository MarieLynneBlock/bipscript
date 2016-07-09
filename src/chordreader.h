#ifndef CHORDREADER_H
#define CHORDREADER_H

#include "midipattern.h"
#include <map>

class ChordReader
{
    std::map<std::string, int *> intervalMap;
public:
    ChordReader();
    Pattern *read(const char *abc, int duration, int division);
};

#endif // CHORDREADER_H
