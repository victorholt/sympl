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

sympl_nsstart

// #define sympl_address_ref(ref) reinterpret_cast<unsigned**>(&ref)
#define sympl_address_ref(ref) ref->_Guid

class SYMPL_API Alloc {
private:
    /// Alloc singleton instance.
    static Alloc* _Instance;

    /// Memory allocated by our alloc class.
    size_t _MemAllocated = 0;

    /// Memory table for keeping track of the size allocated.
    std::unordered_map<std::string, size_t> _MemTable;

    /// Reference name table.
    std::unordered_map<std::string, std::string> _RefTable;

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

        auto guid = xg::newGuid();
        std::stringstream guidStream;
        guidStream << guid;
        ref->_Guid = guidStream.str();

        AddMemAllocated(sizeof(T) + ref->_MemSize);

        auto entry = _MemTable.find(sympl_address_ref(ref));
        assert(entry == _MemTable.end() && "Invalid allocation to memory address!");

        _MemTable[sympl_address_ref(ref)] = sizeof(T) + ref->_MemSize;
        _RefTable[sympl_address_ref(ref)] = ref->GetTypeName();

        return ref;
    }

    //! Free memory from a sympl reference.
    template<class T>
    inline void FreeRef(T*& ref)
    {
        auto entry = _MemTable.find(sympl_address_ref(ref));
    //    assert(entry != _MemTable.end() && "Invalid access to free memory!");

        if (entry == _MemTable.end()) {
            return;
        }

        if (!ref->Free()) {
            return;
        }

        size_t size = entry->second;
        RemoveMemAllocated(size);
        _MemTable.erase(sympl_address_ref(ref));
        _RefTable.erase(sympl_address_ref(ref));

        free(ref->_Data);
        delete ref;
        ref = nullptr;
    }

    //! Returns a string of the references still in memory.
    //! \return string
    inline std::string PrintRefs() {
        std::string memAddresses;
        for (auto entryIt : _MemTable) {
            memAddresses.append(entryIt.first);
            memAddresses.append(" (");
            memAddresses.append(_RefTable[entryIt.first]);
            memAddresses.append(")\n");
        }
        return memAddresses;
    }
};

#define alloc_ref(clazz) Sympl::Alloc::GetInstance()->MallocRef<clazz>(sizeof(clazz))
#define free_ref(type, ref) Sympl::Alloc::GetInstance()->FreeRef<type>(ref)

#define AllocInstance Sympl::Alloc::GetInstance()

sympl_nsend
