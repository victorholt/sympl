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

#include <sympl/core/object.h>
#include <sympl/core/variant.h>
#include <sympl/core/shared_ptr.h>

sympl_nsstart

class MemPool;
class MemPoolRef;
class MemPoolObject;
class MemPoolManager;

class SYMPL_API Registry
{
protected:
    /// Registered entries.
    Urho3D::Vector<SharedPtr<Object>> _Entries;

    //! Constructor.
    Registry() = default;

public:
    //! Destructor.
    ~Registry() {
        Clear();
    }

    //! Returns the singleton for our class.
    //! \return Registry
    static Registry& GetInstance() {
        static Registry instance;
        return instance;
    }

    //! Registers an object.
    //! \param entry
    void Register(Object* entry) {
        // Ensure the entry isn't already registered.
        auto size = _Entries.Size();
        for (unsigned i = 0; i < size; i++) {
            if (_Entries[i]->GetTypeName() == entry->GetTypeName()) {
                return;
            }
        }
        _Entries.Push(entry);
    }

    //! Returns a registered object.
    //! \tparam T
    //! \return T
    template<class T>
    T* Get() {
        auto size = _Entries.Size();
        for (unsigned i = 0; i < size; i++) {
            if (_Entries[i]->GetTypeName() == T::GetTypeNameStatic()) {
                return dynamic_cast<T*>(_Entries[i].Ptr());
            }
        }
        return nullptr;
    }

    void Clear() {
        _Entries.Clear();
    }

public:
    friend MemPool;
    friend MemPoolRef;
    friend MemPoolObject;
    friend MemPoolManager;
};

#define SymplRegistry Registry::GetInstance()

sympl_nsend
