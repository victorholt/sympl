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
#include <sympl/core/shared_ptr.h>

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
    _Buffer = alloc_bytes_array(uchar, SYMPL_STRING_BUFFER_CAPACITY);
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

    free_bytes_array(_Buffer);
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

    free_bytes_array(_Buffer);
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

void StringBuffer::ReplaceStringAt(const char* str, size_t startIndex, size_t length)
{
    size_t len = length == 0 ? strlen(str) : length;
    size_t end = len + startIndex;
    if (end >= _Capacity) {
        Resize(_Capacity);
    }

    if (len == 0) {
        memset(_Buffer + startIndex, 0, _Length);
    } else {
        memcpy(_Buffer + startIndex, str, len);
    }

    if (_Length < end) {
        _Length = end;
    }
}

void StringBuffer::Replace(const char *search, const char *replaceWith)
{
    if (_Length == 0) return;

    std::string buffer;
    buffer = CStr();

    for (std::string::size_type i = 0; (i = buffer.find(search, i)) != std::string::npos;)
    {
        buffer.replace(i, strlen(search), replaceWith);
        i += strlen(replaceWith);
    }

    Clear();
    Append(buffer);
}

// TODO: Fails on following case: Replace2("add", "")
void StringBuffer::Replace2(const char *search, const char *replaceWith)
{
    if (_Length == 0) return;

    if (!Contains(search)) {
        return;
    }

    size_t bufferIndex = 0;
    size_t searchIndex = 0;

    size_t replaceLength = strlen(replaceWith);
    size_t searchLength = strlen(search);
    size_t searchPos = 0;
    int replaceLenDiff = (int)searchLength - (int)replaceLength;

    StringBuffer tmpBuffer;

    while (searchIndex < _Length) {
        if (Get(searchIndex) == search[searchPos]) {
            searchPos++;
        } else {
            searchPos = 0;
        }

        // Replace our string.
        if (searchPos == searchLength) {
            tmpBuffer.ReplaceStringAt(replaceWith, (bufferIndex - searchPos + 1), replaceLength);
            if (replaceLenDiff > 0) {
//                searchIndex += replaceLenDiff - 1;
                bufferIndex -= replaceLenDiff - 1;
                tmpBuffer.ClearAt(bufferIndex);
            }
            searchIndex++;

            bufferIndex = tmpBuffer.Length();
            searchPos = 0;
        }

        // Ensure our search index hasn't gone over the length.
        if (searchIndex >= _Length) {
            break;
        }

        tmpBuffer.AppendByte(Get(searchIndex));

        searchIndex++;
        bufferIndex++;
    }

    // Replace the remainder.
//    if (tmpBuffer)

    ClearAt(tmpBuffer.Length());

    _Length = 0;
    ReplaceStringAt(tmpBuffer.CStr(), 0, tmpBuffer.Length());
}

bool StringBuffer::Contains(const char search)
{
    for (size_t i = 0; i < _Length; i++) {
        if (static_cast<char>(_Buffer[i]) == search) {
            return true;
        }
    }
    return false;
}

bool StringBuffer::Contains(const char* search)
{
    if (strlen(search) > _Length) {
        return false;
    }

    if (strlen(search) == 1) {
        return Contains(*search);
    }

    size_t numChars = strlen(search);
    int searchIndex = 0;

    for (size_t i = 0; i < _Length; i++) {
        if (static_cast<char>(_Buffer[i]) == search[searchIndex]) {
            searchIndex++;
        } else {
            searchIndex = 0;
        }

        if (searchIndex == numChars) {
            return true;
        }
    }

    return false;
}

std::string StringBuffer::SubstrFirstOccurrence(const char c)
{
    std::string ret;
    for (size_t i = 0; i < _Length; i++) {
        if (static_cast<char>(_Buffer[i]) == c) {
            break;
        }
        ret.append(std::string(1, static_cast<char>(_Buffer[i])));
    }

    return ret;
}

std::string StringBuffer::SubstrFirstOccurrence(const char* str)
{
    auto strLen = strlen(str);
    if (strLen > _Length) {
        return CStr();
    }

    if (strLen == 1) {
        return SubstrFirstOccurrence(*str);
    }

    std::string ret;
    size_t numChars = strLen;
    int searchIndex = 0;

    for (size_t i = 0; i < _Length; i++) {
        if (static_cast<char>(_Buffer[i]) == str[searchIndex]) {
            searchIndex++;
        } else {
            searchIndex = 0;
        }

        if (searchIndex == numChars) {
            break;
        }
        ret.append(std::string(1, static_cast<char>(_Buffer[i])));
    }

    return ret;
}

long long StringBuffer::FirstOccurrence(const char c, size_t startIndex, size_t limit)
{
    if (startIndex >= _Length) {
        return -1;
    }

    long long index = 0;
    size_t curLimit = 0;

    for (size_t i = startIndex; i < _Length; i++) {
        if (static_cast<char>(_Buffer[i]) == c) {
            return (index + startIndex);
        }
        index++;
        curLimit++;

        if (limit != 0 && curLimit >= limit) {
            break;
        }
    }

    return -1;
}

long long StringBuffer::FirstOccurrence(const char* str, size_t startIndex, size_t limit)
{
    auto strLen = strlen(str);
    if (strLen > _Length || startIndex >= _Length) {
        return -1;
    }

    if (strLen == 1) {
        return FirstOccurrence(*str);
    }

    long long index = 0;
    size_t curLimit = 0;
    size_t numChars = strLen;
    int searchIndex = 0;

    for (size_t i = startIndex; i < _Length; i++) {
        if (static_cast<char>(_Buffer[i]) == str[searchIndex]) {
            searchIndex++;
        } else {
            searchIndex = 0;
        }

        if (searchIndex == numChars) {
            return ((index + startIndex) - searchIndex + 1);
        }
        index++;
        curLimit++;

        if (limit != 0 && curLimit >= limit) {
            break;
        }
    }

    return -1;
}

bool StringBuffer::PeekSearch(const char* search, size_t startIndex)
{
    size_t endIndex = strlen(search);
    if (endIndex == 0 || startIndex > _Length || (startIndex + endIndex) > _Length) {
        return false;
    }

    size_t searchIndex = 0;
    for (size_t i = startIndex; i < endIndex; i++) {
        if (_Buffer[i] != search[searchIndex]) {
            return false;
        }
        searchIndex++;
    }

    return true;
}

void StringBuffer::Clear()
{
    if (_Length == 0) return;

    _Length = 0;
    memset(_Buffer, 0, _Capacity);
}

void StringBuffer::ClearAt(size_t startIndex)
{
    if (_Length == 0) return;

    _Length = startIndex;
    memset(_Buffer + startIndex, 0, _Length);
}

bool StringBuffer::Release()
{
    free_bytes_array(_Buffer);
    return true;
}
