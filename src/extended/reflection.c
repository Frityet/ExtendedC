/**
 * Copyright (C) 2023 Amrit Bhogal
 *
 * This file is part of ExtendedC.
 *
 * ExtendedC is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ExtendedC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ExtendedC.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "reflection.h"

#include <stdarg.h>
#include <stdbool.h>

struct Buffer {
    size_t length, offset;
    char *data;
};

int dump_to_string(size_t length, char buf[static length], const char *fmt, ...)
{
    static struct Buffer buffer = {0};
    if (buffer.data == NULL) {
        buffer.data = buf;
        buffer.length = EXC_BUFSIZ;
    } else if (buffer.data != buf) {
        buffer.data = buf;
        buffer.offset = 0;
    }

    va_list args;
    va_start(args, fmt);

    //Replace \n with ' '
    char *p = buffer.data + buffer.offset;
    for (size_t i = 0; i < buffer.length - buffer.offset; i++) {
        if (p[i] == '\0') break;
        if (p[i] == '\n') p[i] = ' ';
    }

    int ret = vsnprintf(buffer.data + buffer.offset, buffer.length - buffer.offset, fmt, args);
    buffer.offset += ret;

    va_end(args);

    return ret;
}

int parse_typeinfo(struct TypeInfo *t, const char buffer[static EXC_BUFSIZ])
{


    return 0;
}
