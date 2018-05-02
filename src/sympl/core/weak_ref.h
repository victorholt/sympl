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
#include "sympl_object.h"

sympl_nsstart

template<class T>
class WeakRef
{
private:
    /// Reference to the data.
    T* _Data;

    //! Sets the data.
    //! \param ptr
    void _Set(T* ptr) {
        _Data = ptr;
    }

public:
    //! Constructor.
    explicit WeakRef() {}

    //! Constructor.
    //! \param ptr
    WeakRef(T* ptr) {
        _Set(ptr);
    }

    //! Destructor.
    ~WeakRef() {
    }

    //! Returns the pointer.
    //! \return
    T* Ptr() const { return _Data; }

    //! Operator for assigning long.
    //! \param rhs
    //! \return
    WeakRef<T>& operator =(T* rhs) {
        _Set(rhs);
        return *this;
    }

    //! Operator for assigning long.
    //! \param rhs
    //! \return
    WeakRef<T>& operator =(const WeakRef<T> rhs) {
        _Set(rhs.Ptr());
        return *this;
    }

    //! Dereference access.
    const T *operator->() const {
        assert(_Data != nullptr && _Data->RefCount() > 0 && "Attempted to access empty pointer");
        return *_Data;
    }

    //! Dereference object.
    const T &operator*() const {
        assert(_Data != nullptr && _Data->RefCount() > 0 && "Attempted to access empty pointer");
        return *_Data;
    }

    //! Dereference access.
    T *operator->() {
        assert(_Data != nullptr && _Data->RefCount() > 0 && "Attempted to access empty pointer");
        return *_Data;
    }

    //! Dereference object.
    T &operator*() {
        assert(_Data != nullptr && _Data->RefCount() > 0 && "Attempted to access empty pointer");
        return *_Data;
    }
};

sympl_nsend
