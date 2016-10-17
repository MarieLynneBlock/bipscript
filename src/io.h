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
#ifndef IO_H
#define IO_H

#include <string>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

class File {
    std::string name;
    char *buffer;
    int length;
public:
    File(const char *cname) : buffer(0) {
        name = cname;
    }
    ~File() { delete[] buffer; }
    uintmax_t size() { return fs::file_size(name); }
    bool exists() { return fs::exists(name); }
    bool isFolder() { return fs::is_directory(name); }
    const char *readAll();
};

#endif // IO_H
