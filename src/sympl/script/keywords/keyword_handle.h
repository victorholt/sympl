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
#include <sympl/core/shared_ref.h>

sympl_nsstart

enum class KeywordType : uint8_t
{
    System = 0,
    Custom
};

class SYMPL_API KeywordHandle : public Object
{
    SYMPL_OBJECT(KeywordHandle, Object);

private:
    /// Keyword name.
    std::string _Name;

    /// Keyword type.
    KeywordType _Type;

public:
    //! Constructor.
    KeywordHandle(const char* name, KeywordType type);

    //! Destructor.
    virtual ~KeywordHandle();
};

sympl_nsend
