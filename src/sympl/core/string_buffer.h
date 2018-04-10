/**********************************************************
 * Author:  GameSencha, LLC
 * The MIT License (MIT)
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *  DEALINGS IN THE SOFTWARE.
 *
 **********************************************************/
#ifndef __SYMPL_STRING_BUFFER_H__
#define __SYMPL_STRING_BUFFER_H__

#include "sympl_pch.h"
#include "alloc.h"

#define SYMPL_DEFAULT_STRING_BUFFER_CAPACITY 256

// Base string type for the language.
typedef struct sympl_string_buffer
{
    uchar8      *buffer;
    size_t      length;
    size_t      capacity;
    sympl_ref   *ref;
} sympl_string_buffer;

// Creates a string buffer.
sympl_string_buffer *sympl_string_buffer_create2(uchar8* str, size_t capacity);

// Creates a string buffer.
sympl_string_buffer *sympl_string_buffer_create(uchar8* str);

// Appends a string to the buffer.
void sympl_string_buffer_append(sympl_string_buffer *src, const char8 *str);

// Resize our string buffer.
void sympl_string_buffer_resize(sympl_string_buffer *src, size_t new_capacity);

// Clears a string buffer.
void sympl_string_buffer_clear(sympl_string_buffer *src);

// Frees the string buffer.
void sympl_string_buffer_free(sympl_string_buffer *src);

#endif