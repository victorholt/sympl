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
#include <sympl/core/string_buffer.h>

sympl_namespaces

StringBuffer::StringBuffer(const char *str, size_t capacity)
{
    Init(str, capacity);
}

StringBuffer::StringBuffer(const char *str)
{
    Init(str, SYMPL_STRING_BUFFER_CAPACITY);
}

StringBuffer::StringBuffer()
{
    __Construct();
}

void StringBuffer::__Construct()
{
    Init("", SYMPL_STRING_BUFFER_CAPACITY);
}

void StringBuffer::Init(const char *str, size_t capacity)
{
    // Confirm the string capacity.
    size_t confirmedCapacity = capacity;
    if (strlen(str) > confirmedCapacity) {
        confirmedCapacity = strlen(str) + SYMPL_STRING_BUFFER_CAPACITY;
    }

    // Our new string memory size to allocate.
    size_t strCapacity = sizeof(uchar) * confirmedCapacity;

    // Create/clean up our string.
    _StaticBuffer = alloc_bytes_array(uchar, SYMPL_STRING_BUFFER_CAPACITY);
    _Buffer = _StaticBuffer;
    memset(_Buffer, 0, strCapacity);
    memcpy(_Buffer, str, strlen(str) + 1);

    _Length = strlen(str);
    _Capacity = capacity;
}

void StringBuffer::Prepend(const char *str)
{
    size_t strSize = strlen(str);
    if (strSize == 0) {
        return;
    }

    // Ensure we have enough capacity.
    if ((_Length + strSize) >= _Capacity) {
        Resize(_Length + strSize + SYMPL_STRING_BUFFER_CAPACITY);
    }

    auto tmpBuffer = alloc_bytes_array(uchar, _Capacity);
    memcpy(tmpBuffer + strlen(str), _Buffer, _Length);
    memcpy(tmpBuffer, str, strlen(str));

    if (_UseDynamicBuffer) {
        free(_Buffer);
    } else {
        free_bytes_array(_Buffer);
        _StaticBuffer = tmpBuffer;
    }
    _Buffer = tmpBuffer;

    _Length += strlen(str);
}

void StringBuffer::PrependByte(const char byte)
{
    // Ensure we have enough capacity.
    if ((_Length + 1) >= _Capacity) {
        Resize(_Length + 1 + SYMPL_STRING_BUFFER_CAPACITY);
    }

    auto tmpBuffer = alloc_bytes_array(uchar, _Capacity);
    memset(tmpBuffer, 0, _Capacity);
    memcpy(tmpBuffer + 1, _Buffer, _Length);
    tmpBuffer[0] = static_cast<uchar>(byte);
    _Length += 1;

    if (_UseDynamicBuffer) {
        free(_Buffer);
    } else {
        free_bytes_array(_Buffer);
        _StaticBuffer = tmpBuffer;
    }
    _Buffer = tmpBuffer;
}

void StringBuffer::Append(StringBuffer *str)
{
    Append(str->CStr());
}

void StringBuffer::Append(const std::string str)
{
    Append(str.c_str());
}

void StringBuffer::Append(const char *str)
{
    size_t strSize = strlen(str);
    if (strSize == 0) {
        return;
    }

    // Ensure we have enough capacity.
    if ((_Length + strSize) >= _Capacity) {
        Resize(_Length + strSize + SYMPL_STRING_BUFFER_CAPACITY);
    }

    size_t index = 0;
    while (index < strSize) {
        _Buffer[_Length + index] = static_cast<uchar>(str[index]);
        index++;
    }

    _Length += strlen(str);
}

void StringBuffer::AppendByte(const char byte)
{
    // Ensure we have enough capacity.
    if ((_Length + 2) >= _Capacity) {
        Resize(_Length + 2 + SYMPL_STRING_BUFFER_CAPACITY);
    }

    _Buffer[_Length] = static_cast<uchar>(byte);
    _Length += 1;
}

void StringBuffer::Resize(size_t newCapacity)
{
    // Check if we need to clear everything.
    if (newCapacity <= _Capacity) {
        return;
    } else {
        // We probably need a lot of room for this buffer,
        // so really up the capacity.
        _Capacity = newCapacity + (_Capacity * 5);
    }

    auto *tmpStr = reinterpret_cast<uchar*>(calloc(_Capacity, sizeof(uchar)));

    if (_Length > 0) {
        size_t index = 0;
        while (index < _Length) {
            tmpStr[index] = _Buffer[index];
            index++;
        }
    }

    if (_UseDynamicBuffer) {
        free(_Buffer);
    }

    _Buffer = tmpStr;
    _UseDynamicBuffer = true;
}

void StringBuffer::ReplaceAt(size_t pos, const char* str)
{
    memset(_Buffer + pos, 0, _Capacity - pos);
    memcpy(_Buffer + pos, str, strlen(str));
}

void StringBuffer::Replace(const char *search, const char *replaceWith)
{
    if (_Length == 0) return;

    std::string buffer;
    //buffer.reserve(_Capacity);
    buffer = CStr();

    for (std::string::size_type i = 0; (i = buffer.find(search, i)) != std::string::npos;)
    {
        buffer.replace(i, strlen(search), replaceWith);
        i += strlen(replaceWith);
    }

    Clear();
    Append(buffer);

//
//     StringBuffer findBuffer;
//     findBuffer.Resize(_Capacity);
//
//     StringBuffer tmpBuffer;
//
//     long long tmpBufferIndex = 0;
//     long long capacity = _Capacity;
//     size_t strLen = strlen(search);
//     if (strLen > _Length) {
//         return;
//     }
//
//     for (size_t i = 0; i < _Length; i++) {
//         tmpBuffer.AppendByte(Get(i));
//         tmpBufferIndex++;
//
//         // Ensure we've copied over enough character to check the sequence.
//         if (i < (strLen - 1)) {
//             findBuffer.AppendByte(Get(i));
//             continue;
//         }
//
//         // Shift the characters to the right.
//         for (size_t j = 0; j < strLen - 1; j++) {
//             findBuffer.SetByte(j, Get((i - (strLen - 1)) + j));
//         }
//         findBuffer.SetByte(strLen - 1, Get(i));
//
//         // Compare the current sequence of strings.
//         if (strcmp(findBuffer.CStr(), search) == 0) {
//             size_t replaceIndex = (tmpBufferIndex - strLen);
//             tmpBufferIndex = replaceIndex + strlen(replaceWith);
//
//             // Check if we need to resize the buffer.
//             if ((capacity - tmpBufferIndex) < 0) {
//                 capacity += (tmpBufferIndex * 2) + (capacity * 2);
//                 tmpBuffer.Resize(capacity);
//             }
//
//             tmpBuffer.ReplaceAt(replaceIndex, replaceWith);
//         }
//     }
//
//     Clear();
//     Append(&tmpBuffer);

    /*
    long long capacity = _Capacity;
    size_t strLen = strlen(search);
    if (strLen > _Length) {
        return;
    }

    char8* findBuffer = new char8[strLen + 1];
    memset(findBuffer, 0, strLen + 1);

    char8* tmpStr = new char8[capacity];
    memset(tmpStr, 0, capacity);

    long long tmpBufferIndex = 0;
    long long currentFindBufferIndex = strLen;

    for (size_t i = 0; i < _Length; i++) {
        tmpStr[tmpBufferIndex++] = Get(i);

        // Ensure we've copied over enough character to check the sequence.
        if (i < (strLen - 1)) {
            findBuffer[i] = Get(i);
            continue;
        }

        // Shift the characters to the right.
        for (size_t j = 0; j < strLen - 1; j++) {
            findBuffer[j] = Get((i - (strLen - 1)) + j);
        }
        findBuffer[strLen - 1] = Get(i);

        // Compare the current sequence of strings.
        if (strcmp(findBuffer, search) == 0) {
            size_t replaceIndex = (tmpBufferIndex - strLen);
            tmpBufferIndex = replaceIndex + strlen(replaceWith);

            // Check if we need to resize the buffer.
            if ((capacity - tmpBufferIndex) < 0) {
                capacity += (tmpBufferIndex * 2) + (capacity * 2);

                char8* newBuffer = new char8[capacity];
                memset(newBuffer, 0, capacity);
                memcpy(newBuffer, tmpStr, strlen(tmpStr) + 1);

                delete [] tmpStr;
                tmpStr = newBuffer;
            }

            memset(tmpStr + replaceIndex, 0, capacity - replaceIndex);
            memcpy(tmpStr + replaceIndex, replaceWith, strlen(replaceWith));
        }
    }

    Clear();
    Append(tmpStr);

    delete [] tmpStr;
    delete [] findBuffer;
    */
}

void StringBuffer::Clear()
{
    if (_Length == 0) return;

    _Length = 0;
    memset(_Buffer, 0, _Capacity);
}

bool StringBuffer::Release()
{
    if (_UseDynamicBuffer) {
        free(_Buffer);
    }
    free_bytes_array(_StaticBuffer);

    return true;
}
