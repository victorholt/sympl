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

template<class T>
class WeakRef
{
private:
    /// Reference to the data.
    T* _Data = nullptr;

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

    //! Returns the data reference count.
    //! \return
    unsigned RefCount() const
    {
        if (IsNull()) return 0;
        return _Data->RefCount();
    }

    //! Returns whether or not the reference is valid.
    //! \return bool
    bool IsNull() const {
        return IsNullObject(_Data);
    }

    //! Returns whether or not the reference is valid.
    //! \return bool
    const bool IsValid() const {
        return !IsNull() && (RefCount() > 0);
    }

    //! Returns the pointer.
    //! \return
    T* Ptr() const { return _Data; }

    //! Operator for assigning pointer.
    //! \param rhs
    //! \return
    WeakRef<T>& operator =(T* rhs) {
        _Set(rhs);
        return *this;
    }

    //! Operator for assigning shared pointer.
    //! \param rhs
    //! \return
    WeakRef<T>& operator =(const WeakRef<T>& rhs) {
        _Set(rhs.Ptr());
        return *this;
    }

    //! Operator for testing equality.
    //! \param rhs
    //! \return
    bool operator ==(T* rhs) {
        return _Data == rhs;
    }

    //! Operator for testing equality.
    //! \param rhs
    //! \return
    bool operator ==(const WeakRef<T>& rhs) {
        return _Data == rhs.Ptr();
    }

    //! Dereference access.
    const T *operator->() const {
        assert(IsValid() && "Attempted to access empty pointer");
        return _Data;
    }

    //! Dereference object.
    const T &operator*() const {
        assert(IsValid() && "Attempted to access empty pointer");
        return *_Data;
    }

    //! Dereference access.
    T *operator->() {
        assert(IsValid() && "Attempted to access empty pointer");
        return _Data;
    }

    //! Dereference object.
    T &operator*() {
        assert(IsValid() && "Attempted to access empty pointer");
        return *_Data;
    }
};

sympl_nsend
