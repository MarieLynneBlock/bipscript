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
#ifndef AUDIOCONNECTION_H
#define AUDIOCONNECTION_H

#include "source.h"
#include <atomic>
#include <stdexcept>
#include <cstring>

class AudioConnection;

/**
 * Base for all classes that produce audio
 */
class AudioSource : virtual public Source {
public:
    virtual unsigned int getAudioOutputCount() = 0;
    virtual AudioConnection *getAudioConnection(unsigned int index) = 0;
};

/**
 * Represents a mono audio connection
 */
class AudioConnection {
    static float *dummyBuffer;
    static jack_nframes_t bufferSize;
    AudioSource *source;    
    float *buffer;
public:
    // static
    static void setBufferSize(jack_nframes_t size) {
        if(dummyBuffer) {
            delete[] dummyBuffer;
        }
        dummyBuffer = new float[size];
        bufferSize = size;
    }
    static float *getDummyBuffer() {
        return dummyBuffer;
    }
    // instance
    AudioConnection(AudioSource *source, bool allocate = true)
        : source(source) {
        if(allocate) {
            buffer = new float[bufferSize];
        }
    }
    bool connectsTo(Source *src) {
        return source == src || source->connectsTo(src);
    }
    void setBuffer(float *buffer) { this->buffer = buffer; }
    float *getBuffer() { return buffer; }
    float *getAudio(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time) {
        source->process(rolling, pos, nframes, time);
        return buffer;
    }
    void clear() {
        std::memset(buffer, 0, sizeof(float) * bufferSize);
    }
};

/**
 * Represents an input for a mono audio connection
 */
class AudioConnector {
    std::atomic<AudioConnection *> connection;
public:
    AudioConnector() : connection(0) {}
    void setConnection(AudioConnection *conn, Source *source) {
        if(conn->connectsTo(source)) {
            throw std::logic_error("Cannot connect infinite loop");
        }
        connection.store(conn);
    }
    AudioConnection *getConnection() {
        return connection.load();
    }
};

#endif // AUDIOCONNECTION_H
