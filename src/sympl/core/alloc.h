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

sympl_nsstart

class SYMPL_API Alloc {
private:
    /// Alloc singleton instance.
    static Alloc* _Instance;

    /// Memory allocated by our alloc class.
    size_t _MemAllocated = 0;

    /// Memory table for keeping track of the size allocated.
    std::unordered_map<int*, size_t> _MemTable;

    //! Constructor.
    Alloc() {}

    //! Add to the memory allocated.
    //! \param amount
    inline void AddMemAllocated(size_t amount) { _MemAllocated += amount; }

    //! Remove from the memory allocated.
    //! \param amount
    inline void RemoveMemAllocated(size_t amount) { _MemAllocated -= amount; }

public:
    //! Destructor.
    ~Alloc() {
        if (!IsNullObject(_Instance)) {
            _Instance->_MemTable.clear();
            delete _Instance;
        }
    }

    //! Returns the singleton for our Alloc class.
    //! \return Alloc*
    inline static Alloc* GetInstance() {
        if (IsNullObject(_Instance)) {
            _Instance = new Alloc();
        }
        return _Instance;
    }

    //! Returns our memory allocated.
    //! \return
    inline size_t GetMemAllocated() const { return _MemAllocated; }

    //! Allocate memory to a sympl reference.
    template<class T>
    inline T* MallocRef(size_t size)
    {
        T* ref = new T();
        ref->_MemSize = size;
        ref->_Data = malloc(size);

        AddMemAllocated(sizeof(T) + ref->_MemSize);
        _MemTable[reinterpret_cast<int*>(ref)] = sizeof(T) + ref->_MemSize;

        return ref;
    }

    //! Free memory from a sympl reference.
    template<class T>
    inline void FreeRef(T*& ref)
    {
        if (!ref->Free()) {
            return;
        }

        auto entry = _MemTable.find(reinterpret_cast<int*>(ref));
        if (entry == _MemTable.end()) {
            return;
        }

        size_t size = entry->second;
        RemoveMemAllocated(size);
        _MemTable.erase(entry);

        free(ref->_Data);
        delete ref;
        ref = nullptr;
    }

    //! Allocate memory to a sympl reference.
    template<class T>
    inline T* Malloc(size_t size)
    {
        T *ref = new T[size];

        AddMemAllocated(size);
        _MemTable[reinterpret_cast<int*>(ref)] = size;

        return ref;
    }

    //! Free memory from a sympl reference.
    template<class T>
    inline void Free(T* data)
    {
        size_t size = _MemTable[reinterpret_cast<int*>(data)];
        RemoveMemAllocated(size);

        delete data;
    }

    //! Free memory from a sympl reference.
    template<class T>
    inline void FreeArray(T* data)
    {
        size_t size = _MemTable[reinterpret_cast<int*>(data)];
        RemoveMemAllocated(size);

        delete []data;
    }
};

#define alloc_ref(clazz) Sympl::Alloc::GetInstance()->MallocRef<clazz>(sizeof(clazz))
#define free_ref(type, ref) Sympl::Alloc::GetInstance()->FreeRef<type>(ref)

#define alloc_data(type, size) static_cast<type*>(Sympl::Alloc::GetInstance()->Malloc<type>(size))
#define free_data(type, data) Sympl::Alloc::GetInstance()->Free<type>(data)
#define free_data_array(type, data) Sympl::Alloc::GetInstance()->FreeArray<type>(data)

#define AllocInstance Sympl::Alloc::GetInstance()

sympl_nsend