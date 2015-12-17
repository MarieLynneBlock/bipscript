#ifndef MIDITUNE_H
#define MIDITUNE_H

#include "midipattern.h"
#include "timesignature.h"
#include <string>

class MidiTune
{
    std::string title;
    Pattern *tracks;
    uint32_t numTracks;
    TimeSignature timeSignature;
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
    uint32_t getTrackCount() {
        return numTracks;
    }
    Pattern *getTrack(uint32_t number);
    void setTimeSignature(float num, float denom) {
        timeSignature = TimeSignature(true, num, denom);
    }
    TimeSignature *getTimeSignature() {
        return new TimeSignature(timeSignature);
    }
};

#endif // MIDITUNE_H
