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
sympl_namespaces

StringBuffer::StringBuffer(const char8 *str, size_t capacity)
{
    Init(str, capacity);
}

StringBuffer::StringBuffer(const char8 *str)
{
    Init(str, SYMPL_STRING_BUFFER_CAPACITY);
}

StringBuffer::StringBuffer()
{
    Init("", SYMPL_STRING_BUFFER_CAPACITY);
}

StringBuffer::~StringBuffer()
{
    free_data_array(uchar8, _Buffer);
}

void StringBuffer::Init(const char8 *str, size_t capacity)
{
    // Confirm the string capacity.
    size_t confirmedCapacity = capacity;
    if (strlen(str) > confirmedCapacity) {
        confirmedCapacity = strlen(str) + SYMPL_STRING_BUFFER_CAPACITY;
    }

    // Our new string memory size to allocate.
    size_t strCapacity = sizeof(uchar8) * confirmedCapacity;

    // Create/clean up our string.
    _Buffer = alloc_data(uchar8, strCapacity);
    memset(_Buffer, 0, strCapacity);
    memcpy(_Buffer, str, strlen(str) + 1);

    _Length = strlen(str);
    _Capacity = capacity;
}

void StringBuffer::Append(const char8 *str)
{
    size_t strSize = strlen(str);
    if (strSize == 0) {
        return;
    }

    // Ensure we have enough capacity.
    if ((_Length + strSize) >= _Capacity) {
        Resize(_Length + strSize + SYMPL_STRING_BUFFER_CAPACITY);
    }

    memcpy(_Buffer + _Length, str, strlen(str));
    _Length += strlen(str);
}

void StringBuffer::AppendByte(const char8 byte)
{
    // Ensure we have enough capacity.
    if ((_Length + 2) >= _Capacity) {
        Resize(_Length + 2 + SYMPL_STRING_BUFFER_CAPACITY);
    }

    _Buffer[_Length] = static_cast<uchar8>(byte);
    _Length += 1;
}

void StringBuffer::Resize(size_t newCapacity)
{
    // Check if we need to clear everything.
    if (newCapacity < _Capacity) {
        free_data_array(uchar8, _Buffer);
        _Capacity = newCapacity;

        _Buffer = alloc_data(uchar8, _Capacity);
        memset(_Buffer, 0, _Capacity);
        return;
    }

    uchar8 *tmpStr = alloc_data(uchar8, newCapacity);
    memset(tmpStr, 0, newCapacity);

    if (_Length > 0) {
        memcpy(tmpStr, _Buffer, _Length);
    }

    free_data_array(uchar8, _Buffer);
    _Buffer = tmpStr;
    _Capacity = newCapacity;
}

void StringBuffer::Clear()
{
    _Length = 0;
    memset(_Buffer, 0, _Capacity);
}
