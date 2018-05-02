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

#include "sympl_pch.h"
#include "alloc.h"

sympl_nsstart

class SYMPL_API Ref {
protected:
    /// Reference to our data that has been allocated.
    void *_Data;

    /// Current reference count before we can delete the reference.
    unsigned _RefCount = 0;

    /// Size of the memory.
    size_t _MemSize = 0;

    /// Allow Alloc access to our data storage.
    friend Alloc;

    //! Returns the allocated memory data.
    //! \return
    void *GetMemData() const { return _Data; }

public:
    // Attempts to dec the reference count.
    virtual ~Ref() {
    }

    //! Adds to the reference count.
    inline void AddRef() { _RefCount++; }

    //! Returns the reference count.
    //! \return
    inline unsigned RefCount() const { return _RefCount; }

    //! Attempts to free the reference object.
    virtual bool Free();
};

sympl_nsend
