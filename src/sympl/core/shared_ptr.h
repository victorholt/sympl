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
#include <sympl/core/mempool.h>
#include <sympl/core/ref.h>

sympl_nsstart

template<class T>
class SharedPtr
{
private:
    /// Reference to the data.
    T* _Data = nullptr;

    //! Sets the data.
    //! \param ptr
    void _Set(T* ptr)
    {
        if (IsNullObject(ptr)) {
            return;
        } else if (ptr == _Data) {
            return;
        }

        // Release the current pointer reference.
        Release();

        // Set the new pointer reference.
        _Data = ptr;
        _AddRef();
    }

    //! Adds to the reference count.
    void _AddRef()
    {
        if (IsNull()) return;
        _Data->AddRef();
    }

public:
    //! Constructor.
    explicit SharedPtr() noexcept { _AddRef(); }

    //! Constructor.
    //! \param ptr
    SharedPtr(T* ptr) noexcept {
        _Set(ptr);
    }

    //! Constructor.
    //! \param rhs
    SharedPtr(const SharedPtr<T>& rhs) noexcept {
        _Set(rhs.Ptr());
    }

    //! Destructor.
    ~SharedPtr() noexcept {
        Release();
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

    //! Attempts to release the object.
    void Release()
    {
        if (IsNull()) {
            return;
        }
        mem_free_ref(T, _Data);
        _Data = nullptr;
    }

    //! Operator for assigning pointer.
    //! \param rhs
    //! \return
    SharedPtr<T>& operator =(T* rhs) {
        _Set(rhs);
        return *this;
    }

    //! Operator for assigning shared pointer.
    //! \param rhs
    //! \return
    SharedPtr<T>& operator =(const SharedPtr<T>& rhs) {
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
    bool operator ==(const SharedPtr<T>& rhs) {
        return _Data == rhs.Ptr();
    }

    //! Dereference access.
    const T *operator->() const {
        sympl_assert(IsValid(), "Attempted to access empty pointer");
        return _Data;
    }

    //! Dereference object.
    const T &operator*() const {
        sympl_assert(IsValid(), "Attempted to access empty pointer");
        return *_Data;
    }

    //! Dereference access.
    T *operator->() {
        if (!IsValid()) {
            sympl_assert(false, "Attempted to access empty pointer");
        }
        return _Data;
    }

    //! Dereference object.
    T &operator*() {
        sympl_assert(IsValid(), "Attempted to access empty pointer");
        return *_Data;
    }
};

sympl_nsend
