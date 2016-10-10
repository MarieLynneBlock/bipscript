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
#ifndef MEMORYPOOL_H
#define MEMORYPOOL_H

#include <stddef.h>
#include "tlsf.h"

class MemoryPool
{
    tlsf_t tlsf;
public:
    MemoryPool(size_t size) {
        void *buff = ::operator new(size);
        tlsf = tlsf_create_with_pool(buff, size);
    }
    static MemoryPool &processPool() {
        static MemoryPool instance(32768);
        return instance;
    }
    ~MemoryPool() { tlsf_destroy(tlsf); }
    void *malloc(size_t size) {
        // printf("malloc, ID of this thread is: %u\n", (unsigned int)pthread_self());
        return tlsf_malloc(tlsf, size);
    }
    void free(void *p) {
        // printf("free, ID of this thread is: %u\n", (unsigned int)pthread_self());
        tlsf_free(tlsf, p);
    }
};

#endif // MEMORYPOOL_H
