#ifndef MMLREADER_H
#define MMLREADER_H

#include "mml.h"
#include "midipattern.h"

class MMLReader
{
    MML mml;
    MML_OPTION mmlopt;
    Pattern *pattern;
    Position currentPosition;
    uint32_t ticksPerBar;
    // for chords
    static const int MAX_CHORD_SIZE = 8;
    uint8_t chordTone[MAX_CHORD_SIZE];
    uint8_t chordSize;
public:
    MMLReader();
    void processNote(MML_INFO *p);
    void processRest(MML_INFO *p);
    Pattern *read(const char *mml);
};

#endif // MMLREADER_H
