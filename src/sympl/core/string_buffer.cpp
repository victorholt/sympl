//
// GameSencha, LLC 2/2/24.
//
#include "string_buffer.hpp"
SymplNamespace

StringBuffer::StringBuffer(const char *str, size_t capacity)
{
    init(str, capacity);
}

StringBuffer::StringBuffer(const char *str)
{
    init(str, SYMPL_STRING_BUFFER_CAPACITY);
}

StringBuffer::StringBuffer()
{
    __construct();
}

void StringBuffer::__construct()
{
    init("", SYMPL_STRING_BUFFER_CAPACITY);
}

void StringBuffer::init(const char *str, size_t capacity)
{
    // Confirm the string capacity.
    size_t confirmedCapacity = capacity;
    if (strlen(str) > confirmedCapacity) {
        confirmedCapacity = strlen(str) + SYMPL_STRING_BUFFER_CAPACITY;
    }

    // Our new string memory size to allocate.
    size_t strCapacity = sizeof(uchar) * confirmedCapacity;

    // Create/clean up our string.
    _buffer = alloc_bytes_array(uchar, SYMPL_STRING_BUFFER_CAPACITY);
    memset(_buffer, 0, strCapacity);
    memcpy(_buffer, str, strlen(str) + 1);

    _length = strlen(str);
    _capacity = capacity;
}

void StringBuffer::prepend(const char *str)
{
    size_t strSize = strlen(str);
    if (strSize == 0) {
        return;
    }

    // Ensure we have enough capacity.
    if ((_length + strSize) >= _capacity) {
        resize_string(_length + strSize + SYMPL_STRING_BUFFER_CAPACITY);
    }

    auto tmpBuffer = alloc_bytes_array(uchar, _capacity);
    memcpy(tmpBuffer + strlen(str), _buffer, _length);
    memcpy(tmpBuffer, str, strlen(str));

    free_bytes_array(_buffer);
    _buffer = tmpBuffer;

    _length += strlen(str);
}

void StringBuffer::prepend_byte(const char byte)
{
    // Ensure we have enough capacity.
    if ((_length + 1) >= _capacity) {
        resize_string(_length + 1 + SYMPL_STRING_BUFFER_CAPACITY);
    }

    auto tmpBuffer = alloc_bytes_array(uchar, _capacity);
    memset(tmpBuffer, 0, _capacity);
    memcpy(tmpBuffer + 1, _buffer, _length);
    tmpBuffer[0] = static_cast<uchar>(byte);
    _length += 1;

    free_bytes_array(_buffer);
    _buffer = tmpBuffer;
}

void StringBuffer::append(StringBuffer *str)
{
    append(str->cstr());
}

void StringBuffer::append(const std::string str)
{
    append(str.c_str());
}

void StringBuffer::append(const char *str)
{
    size_t strSize = strlen(str);
    if (strSize == 0) {
        return;
    }

    // Ensure we have enough capacity.
    if ((_length + strSize) >= _capacity) {
        resize_string(_length + strSize + SYMPL_STRING_BUFFER_CAPACITY);
    }

    size_t index = 0;
    while (index < strSize) {
        _buffer[_length + index] = static_cast<uchar>(str[index]);
        index++;
    }

    _length += strlen(str);
}

void StringBuffer::append_byte(const char byte)
{
    // Ensure we have enough capacity.
    if ((_length + 2) >= _capacity) {
        resize_string(_length + 2 + SYMPL_STRING_BUFFER_CAPACITY);
    }

    _buffer[_length] = static_cast<uchar>(byte);
    _length += 1;
}

void StringBuffer::set_string(const char* str)
{
    clear();
    append(str);
}

void StringBuffer::resize_string(size_t newCapacity)
{
    // Check if we need to clear everything.
    if (newCapacity <= _capacity) {
        return;
    } else {
        // We probably need a lot of room for this buffer,
        // so really up the capacity.
        _capacity = newCapacity + (_capacity * 5);
    }

    auto *tmpStr = reinterpret_cast<uchar*>(calloc(_capacity, sizeof(uchar)));

    if (_length > 0) {
        size_t index = 0;
        while (index < _length) {
            tmpStr[index] = _buffer[index];
            index++;
        }
    }

    if (_use_dynamic_buffer) {
        free(_buffer);
    }

    _buffer = tmpStr;
    _use_dynamic_buffer = true;
}

void StringBuffer::replace_string_at(const char* str, size_t startIndex, size_t length)
{
    size_t len = length == 0 ? strlen(str) : length;
    size_t end = len + startIndex;
    if (end >= _capacity) {
        resize_string(_capacity);
    }

    if (len == 0) {
        memset(_buffer + startIndex, 0, _length);
    } else {
        memcpy(_buffer + startIndex, str, len);
    }

    if (_length < end) {
        _length = end;
    }
}

void StringBuffer::replace_string(const char *search, const char *replaceWith)
{
    if (_length == 0) return;

    std::string buffer;
    buffer = cstr();

    for (std::string::size_type i = 0; (i = buffer.find(search, i)) != std::string::npos;)
    {
        buffer.replace(i, strlen(search), replaceWith);
        i += strlen(replaceWith);
    }

    clear();
    append(buffer);
}

// TODO: Fails on following case: Replace2("add", "")
void StringBuffer::safe_replace_string(const char *search, const char *replaceWith)
{
    if (_length == 0) return;

    if (!contains(search)) {
        return;
    }

    size_t bufferIndex = 0;
    size_t searchIndex = 0;

    size_t replaceLength = strlen(replaceWith);
    size_t searchLength = strlen(search);
    size_t searchPos = 0;
    int replaceLenDiff = (int)searchLength - (int)replaceLength;

    StringBuffer tmpBuffer;

    while (searchIndex < _length) {
        if (get_char_at(searchIndex) == search[searchPos]) {
            searchPos++;
        } else {
            searchPos = 0;
        }

        // Replace our string.
        if (searchPos == searchLength) {
            tmpBuffer.replace_string_at(replaceWith, (bufferIndex - searchPos + 1), replaceLength);
            if (replaceLenDiff > 0) {
//                searchIndex += replaceLenDiff - 1;
                bufferIndex -= replaceLenDiff - 1;
                tmpBuffer.clear_at(bufferIndex);
            }
            searchIndex++;

            bufferIndex = tmpBuffer.length();
            searchPos = 0;
        }

        // Ensure our search index hasn't gone over the length.
        if (searchIndex >= _length) {
            break;
        }

        tmpBuffer.append_byte(get_char_at(searchIndex));

        searchIndex++;
        bufferIndex++;
    }

    // Replace the remainder.
//    if (tmpBuffer)

    clear_at(tmpBuffer.length());

    _length = 0;
    replace_string_at(tmpBuffer.cstr(), 0, tmpBuffer.length());
}

bool StringBuffer::contains(const char search)
{
    for (size_t i = 0; i < _length; i++) {
        if (static_cast<char>(_buffer[i]) == search) {
            return true;
        }
    }
    return false;
}

bool StringBuffer::contains(const char* search)
{
    if (strlen(search) > _length) {
        return false;
    }

    if (strlen(search) == 1) {
        return contains(*search);
    }

    size_t numChars = strlen(search);
    int searchIndex = 0;

    for (size_t i = 0; i < _length; i++) {
        if (static_cast<char>(_buffer[i]) == search[searchIndex]) {
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

bool StringBuffer::is_within(const std::vector<const char*>& searchList)
{
    for (const auto& str : searchList)
    {
        if (equals(str)) {
            return true;
        }
    }

    return false;
}

std::string StringBuffer::substr_first_occurrence(const char c)
{
    std::string ret;
    for (size_t i = 0; i < _length; i++) {
        if (static_cast<char>(_buffer[i]) == c) {
            break;
        }
        ret.append(std::string(1, static_cast<char>(_buffer[i])));
    }

    return ret;
}

std::string StringBuffer::substr_first_occurrence(const char* str)
{
    auto strLen = strlen(str);
    if (strLen > _length) {
        return cstr();
    }

    if (strLen == 1) {
        return substr_first_occurrence(*str);
    }

    std::string ret;
    size_t numChars = strLen;
    int searchIndex = 0;

    for (size_t i = 0; i < _length; i++) {
        if (static_cast<char>(_buffer[i]) == str[searchIndex]) {
            searchIndex++;
        } else {
            searchIndex = 0;
        }

        if (searchIndex == numChars) {
            break;
        }
        ret.append(std::string(1, static_cast<char>(_buffer[i])));
    }

    return ret;
}

long long StringBuffer::first_occurrence(const char c, size_t startIndex, size_t limit)
{
    if (startIndex >= _length) {
        return -1;
    }

    long long index = 0;
    size_t curLimit = 0;

    for (size_t i = startIndex; i < _length; i++) {
        if (static_cast<char>(_buffer[i]) == c) {
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

long long StringBuffer::first_occurrence(const char* str, size_t startIndex, size_t limit)
{
    auto strLen = strlen(str);
    if (strLen > _length || startIndex >= _length) {
        return -1;
    }

    if (strLen == 1) {
        return first_occurrence(*str);
    }

    long long index = 0;
    size_t curLimit = 0;
    size_t numChars = strLen;
    int searchIndex = 0;

    for (size_t i = startIndex; i < _length; i++) {
        if (static_cast<char>(_buffer[i]) == str[searchIndex]) {
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

bool StringBuffer::peek_search(const char* search, size_t startIndex)
{
    size_t endIndex = startIndex + strlen(search);
    if (endIndex == 0 || startIndex > _length || endIndex > _length) {
        return false;
    }

    size_t searchIndex = 0;
    for (size_t i = startIndex; i < endIndex; i++) {
        if (_buffer[i] != search[searchIndex]) {
            return false;
        }
        searchIndex++;
    }

    return true;
}

bool StringBuffer::starts_with(const char* search)
{
    auto strLen = strlen(search);
    if (strLen > _length) {
        return false;
    }

    for (size_t i = 0; i < strLen; i++) {
        if (_buffer[i] != search[i]) {
            return false;
        }
    }

    return true;
}

bool StringBuffer::ends_with(const char* search)
{
    auto strLen = strlen(search);
    if (strLen > _length) {
        return false;
    }

    for (size_t i = 0; i < strLen; i++) {
        size_t checkIndex = _length - (strLen - i);
        char c = _buffer[checkIndex];
        if (_buffer[checkIndex] != search[i]) {
            return false;
        }
    }

    return true;
}

void StringBuffer::ltrim()
{
    std::string TrimString = cstr();
    TrimString.erase(TrimString.begin(), std::find_if(TrimString.begin(), TrimString.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    set_string(TrimString.c_str());
}

void StringBuffer::rtrim()
{
    std::string TrimString = cstr();
    TrimString.erase(std::find_if(TrimString.rbegin(), TrimString.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), TrimString.end());
    set_string(TrimString.c_str());
}

void StringBuffer::trim()
{
    ltrim();
    rtrim();
}

void StringBuffer::clear()
{
    if (_length == 0) return;

    _length = 0;
    memset(_buffer, 0, _capacity);
}

void StringBuffer::clear_at(size_t startIndex)
{
    if (_length == 0) return;

    _length = startIndex;
    memset(_buffer + startIndex, 0, _length);
}

bool StringBuffer::destroy()
{
    free_bytes_array(_buffer);
    return true;
}

StringBuffer& StringBuffer::operator=(const char* Str)
{
    set_string(Str);
    return *this;
}
StringBuffer& StringBuffer::operator=(const StringBuffer& Str)
{
    if (this == &Str) {
        return *this;
    }

    set_string(Str.cstr());
    return *this;
}
StringBuffer& StringBuffer::operator=(const SharedPtr<StringBuffer>& Str)
{
    if (this == Str.ptr()) {
        return *this;
    }

    set_string(Str->cstr());

    return *this;
}
StringBuffer& StringBuffer::operator=(const std::string& Str)
{
    set_string(Str.c_str());
    return *this;
}