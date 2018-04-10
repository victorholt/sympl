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
#include "string_buffer.h"

sympl_string_buffer *sympl_string_buffer_create2(uchar8* str, size_t capacity)
{
    // Create our reference object.
    sympl_string_buffer *src = NULL;
    sympl_ref *ref = sympl_new_ref(&str, sizeof(sympl_string_buffer));
return src;
    // Confirm the string capacity.
    size_t confirmed_capacity = capacity;
    if (strlen(str) > confirmed_capacity) {
        confirmed_capacity = strlen(str) + SYMPL_DEFAULT_STRING_BUFFER_CAPACITY;
    }

    // Our new string memory size to allocate.
    size_t str_capacity = sizeof(uchar8) * confirmed_capacity;

    // Create/clean up our string.
    src->buffer = sympl_new(str_capacity);
    memset(src->buffer, 0, str_capacity);
    memcpy(src->buffer, str, strlen(str));

    src->length = strlen(str);
    src->capacity = capacity;
    return src;
}

sympl_string_buffer *sympl_string_buffer_create(uchar8* str)
{
    return sympl_string_buffer_create2(str, SYMPL_DEFAULT_STRING_BUFFER_CAPACITY);
}

void sympl_string_buffer_append(sympl_string_buffer *src, const char8 *str)
{
    size_t str_size = strlen(str);
    if (str_size == 0) {
        return;
    }

    // Ensure we have enough capacity.
    if ((src->length + str_size) >= src->capacity) {
        sympl_string_buffer_resize(src, src->length + str_size + SYMPL_DEFAULT_STRING_BUFFER_CAPACITY);
    }

    memcpy(src->buffer + src->length, str, strlen(str));
    src->length += strlen(str);
}

void sympl_string_buffer_resize(sympl_string_buffer *src, size_t new_capacity)
{
    uchar8 *tmp_str = sympl_new(new_capacity);
    memset(tmp_str, 0, new_capacity);
    memcpy(tmp_str, src->buffer, src->length);

    sympl_delete(src->buffer, src->capacity);
    src->buffer = tmp_str;
    src->capacity = new_capacity;
}

void sympl_string_buffer_clear(sympl_string_buffer *src)
{

}

void sympl_string_buffer_free(sympl_string_buffer *src)
{

}
