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
#pragma

#include "../core/sympl_pch.h"
#include "../core/sympl_object.h"

sympl_nsstart

class StringBuffer;

#define SYMPL_STRING_BUFFER_CAPACITY 512

class SYMPL_API FileWriter : public Object
{
private:
    /// Buffer for holding the string.
    uchar8      *_Buffer = nullptr;
    /// Current length of the string.
    size_t      _Length = 0;
    /// Capacity for the string
    size_t      _Capacity = SYMPL_STRING_BUFFER_CAPACITY;

public:
    //! Constructor.
    FileWriter();

    //! Destructor.
    ~FileWriter();

    //! Appends a string to the current buffer.
    //! \param str
    void Append(const char8 *str);

    //! Writes to the file.
    //! \param str
    void Write(const char *str);

    //! Cleans out the file.
    void Clean();
};

sympl_nsend