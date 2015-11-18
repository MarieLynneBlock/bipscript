#ifndef MIDITUNE_H
#define MIDITUNE_H

#include "midipattern.h"
#include <string>

class MidiTune
{
    std::string title;
    Pattern *tracks;
    uint32_t numTracks;
public:
    MidiTune(uint32_t numTracks) :
        numTracks(numTracks) {
        tracks = new Pattern[numTracks];
    }
    void setTitle(const char *t) { title = t; }
    const char *getTitle() { return title.c_str(); }
    void addMidiEvent(uint32_t tracknum, MidiEvent *event) {
        tracks[tracknum].addEvent(event);
    }
    Pattern *getTrack(uint32_t number);
};

#endif // MIDITUNE_H
