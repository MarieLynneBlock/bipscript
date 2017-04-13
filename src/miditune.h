#ifndef MIDITUNE_H
#define MIDITUNE_H

#include "midipattern.h"
#include "timesignature.h"
#include <string>

namespace bipscript {
namespace midi {

class MidiTune
{
    std::string title;
    Pattern *tracks;
    uint32_t numTracks;
    transport::TimeSignature timeSignature;
public:
    MidiTune(uint32_t numTracks) :
        numTracks(numTracks) {
        tracks = new Pattern[numTracks];
    }
    ~MidiTune() {
        delete[] tracks;
    }
    void setTitle(const char *t) { title = t; }
    const char *getTitle() { return title.c_str(); }
    void addMidiNote(uint32_t tracknum, Note &note, Position &position) {
        tracks[tracknum].addNote(note, position);
    }
    uint32_t trackCount() {
        return numTracks;
    }
    Pattern *track(uint32_t number);
    void setTimeSignature(float num, float denom) {
        timeSignature = transport::TimeSignature(true, num, denom);
    }
    transport::TimeSignature *getTimeSignature() {
        return new transport::TimeSignature(timeSignature);
    }
};

}}

#endif // MIDITUNE_H
