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
#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "listable.h"

#include <jack/types.h>

class Processor : public Listable
{
    jack_nframes_t processedUntil;
public:
    Processor() : processedUntil(0) {}
    /**
     *
     * Runs in the process thread.
     */
    void process(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time) {
        if(processedUntil >= time) {
            return;
        }
        doProcess(rolling, pos, nframes, time);
        processedUntil = time;
    }
    /**
     * Called when a reposition has been requested so objects can flush/recycle queued events.
     *
     * Runs in the process thread.
     */
    virtual void reposition() = 0;
    /**
     * Called after a reposition has been requested until all objects return true.
     *
     * Returns true when the reposition is complete.
     *
     * Runs in the process thread.
     */
    virtual bool repositionComplete() { return true; }
protected:
    virtual void doProcess(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time) = 0;
};

#endif // PROCESSOR_H
