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

#define SYMPL_STRING_BUFFER_CAPACITY 512

class SYMPL_API StringBuffer : public Object
{
private:
    /// Buffer for holding the string.
    uchar8      *_Buffer = nullptr;
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

    //! Appends a string to the current buffer.
    //! \param str
    void Append(const char8 *str);

    //! Appends a string to the current buffer.
    //! \param str
    void AppendByte(const char8 byte);

    //! Resizes the string buffer.
    //! \param newCapacity
    void Resize(size_t newCapacity);

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

    //! Returns the char at a given location.
    //! \param place
    //! \return
    const char Get(size_t location) {
        if (location > Length()) {
            return '\0';
        }
        return _Buffer[location];
    }
};

sympl_nsend
