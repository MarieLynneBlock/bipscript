#include "miditune.h"


Pattern *MidiTune::track(uint32_t tracknum)
{
    if(!tracknum) {
        throw std::logic_error("there is no track zero");
    }
    if(tracknum > numTracks) {
        throw std::logic_error("track number too high");
    }
    return &tracks[tracknum - 1];
}
