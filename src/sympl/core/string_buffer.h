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
#pragma once

#include <sympl/core/sympl_pch.h>
#include <sympl/core/sympl_object.h>

sympl_nsstart

#define SYMPL_STRING_BUFFER_CAPACITY 1512

class SYMPL_API StringBuffer : public Object
{
    SYMPL_OBJECT(StringBuffer, Object);

private:
    /// Buffer for holding the string.
    uchar8      *_Buffer = nullptr;

    /// Static buffer of pre-allocated bytes.
    uchar8      _StaticBuffer[SYMPL_STRING_BUFFER_CAPACITY];

    /// Flag to switch to the dynamic buffer.
    bool        _UseDynamicBuffer = false;

    /// Current length of the string.
    size_t      _Length = 0;
    /// Capacity for the string
    size_t      _Capacity = SYMPL_STRING_BUFFER_CAPACITY;

    //! Initializes the string buffer.
    //! \param str
    //! \param capacity
    void Init(const char8 *str, size_t capacity);

public:
    //! Constructor.
    //! \param str
    //! \param capacity
    StringBuffer(const char8 *str, size_t capacity);

    //! Constructor.
    //! \param str
    StringBuffer(const char8 *str);

    //! Constructor.
    StringBuffer();

    //! Destructor.
    virtual ~StringBuffer();

    //! Prepends a string to the current buffer.
    //! \param str
    void Prepend(const char8 *str);

    //! Prepends a string to the current buffer.
    //! \param str
    void PrependByte(const char8 byte);

    //! Appends a string to the current buffer.
    //! \param str
    void Append(StringBuffer *str);

    //! Appends a string to the current buffer.
    //! \param str
    void Append(const std::string str);

    //! Appends a string to the current buffer.
    //! \param str
    void Append(const char8 *str);

    //! Appends a string to the current buffer.
    //! \param str
    void AppendByte(const char8 byte);

    //! Resizes the string buffer.
    //! \param newCapacity
    void Resize(size_t newCapacity);

    // Sets/replaces a given set of characters at the given location
    //! \param pos
    //! \param str
    void ReplaceAt(size_t pos, const char8* str);

    //! Replaces occurances in the string.
    //! \param search
    //! \param replaceWith
    void Replace(const char8 *search, const char8 *replaceWith);

    //! Clears out the string buffer.
    void Clear();

    //! Returns the string buffer.
    //! \return uchar8*
    inline const uchar8* Str() const { return _Buffer; }

    //! Returns the byte value of the buffer.
    //! \return
    inline const char8* CStr() const { return reinterpret_cast<char8*>(_Buffer); }

    //! Returns the length of the string.
    //! \return size_t
    inline size_t Length() const { return _Length; }

    //! Returns the capacity of the string.
    //! \return size_t
    inline size_t Capacity() const { return _Capacity; }

    //! Returns whether or not a string equals the buffer's string.
    //! \return bool
    inline bool Equals(StringBuffer* buffer) { return Equals(buffer->CStr()); }

    //! Returns whether or not a string equals the buffer's string.
    //! \return bool
    inline bool Equals(const char* compare) { return (strcmp(CStr(), compare) == 0); }

    //! Returns the last byte in the buffer.
    //! \return char
    inline char8 FirstByte() {
        if (_Length == 0) {
            return '\0';
        }
        return reinterpret_cast<char8*>(_Buffer)[0];
    }

    //! Returns the last byte in the buffer.
    //! \return char
    inline char8 LastByte() {
        if (_Length == 0) {
            return '\0';
        }
        return reinterpret_cast<char8*>(_Buffer)[_Length - 1];
    }

    //! Sets a byte at a given location.
    //! \param location
    //! \param byte
    inline void SetByte(size_t location, const char8 byte) {
        if (location >= _Capacity) {
            Resize(_Capacity + 5);
        }
        _Buffer[location] = byte;
    }

    //! Returns the char at a given location.
    //! \param place
    //! \return
    inline const char Get(size_t location) {
        if (location > Length()) {
            return '\0';
        }
        return _Buffer[location];
    }

    //! Evaluates whether two string buffers are equal.
    //! \return bool
    bool operator == (StringBuffer* rhs) {
        return Equals(rhs->CStr());
    }

    //! Evaluates whether two string buffers are equal.
    //! \return bool
    bool operator == (const std::string& rhs) {
        return Equals(rhs.c_str());
    }

    //! Evaluates whether two string buffers are equal.
    //! \return bool
    bool operator == (const char8* rhs) {
        return Equals(rhs);
    }
};

sympl_nsend
