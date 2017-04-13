#include "mmlreader.h"
#include "audioengine.h"

namespace bipscript {
namespace midi {

static void mml_callback(MML_INFO *p, void *extobj)
{
  MMLReader *mmlReader = (MMLReader*)extobj;
  switch (p->type) {
    case MML_TYPE_NOTE:
      mmlReader->processNote(p);
      break;
    case MML_TYPE_REST:
      mmlReader->processRest(p);
      break;
  }
}

MMLReader::MMLReader() : chordSize(0)
{
    mml_init(&mml, mml_callback, this);
    MML_OPTION_INITIALIZER_DEFAULT(&mmlopt);
}

void MMLReader::processNote(MML_INFO *p)
{
    MML_ARGS_NOTE *mmlNote = &(p->args.note);
    // printf("note number %d ticks %d/%d\n", args->number, args->ticks, ticksPerBar);
    if(mmlNote->ticks) {
        Duration duration(0, mmlNote->ticks, ticksPerBar);
        // add waiting chord if one
        for(int i = 0; i < chordSize; i++) {
            Note chordNote(chordTone[i], 127, duration);   // TODO: velocity as parameter
            pattern->addNote(chordNote, currentPosition);
        }
        Note note(mmlNote->number, 127, duration);   // TODO: velocity as parameter
        pattern->addNote(note, currentPosition);
        currentPosition += duration;
        // reset chord
        chordSize = 0;
    } else {
        // zero length note is part of a chord
        if(chordSize < MAX_CHORD_SIZE) {
            chordTone[chordSize++] = mmlNote->number;
        }
    }
}

void MMLReader::processRest(MML_INFO *p)
{  
    MML_ARGS_REST *note = &(p->args.rest);
    currentPosition += Duration(0, note->ticks, ticksPerBar);
    // reset chord
    chordSize = 0;
}

Pattern *MMLReader::read(const char *text)
{
    pattern = new Pattern();
    currentPosition = Position(1, 0, 1);
    transport::TimeSignature &time = AudioEngine::instance().getTimeSignature();
    if(time.isValid()) {
        ticksPerBar = mmlopt.bticks * time.getNumerator() * 4 / time.getDenominator();
    } else {
        ticksPerBar = mmlopt.bticks * 4;
    }
    mml_setup(&mml, &mmlopt, (char *)text);
    while (mml_fetch(&mml) == MML_RESULT_OK) {}
    return pattern;
}

}}