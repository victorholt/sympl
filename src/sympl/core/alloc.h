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
#include <fmt/format.h>

sympl_nsstart

#define MAX_RESERVE_BLOCKS              1000
#define MAX_RESERVE_BLOCK_SIZE          1000
#define sympl_address_ref(ref) ref->_MemAddress

struct ReservedMemBlock
{
    void*       Data;
    size_t      Size;
    bool        Free;
}

class SYMPL_API Alloc {
private:
    /// Alloc singleton instance.
    static Alloc* _Instance;

    /// Memory allocated by our alloc class.
    size_t _MemAllocated = 0;

    /// Next number for the new generated mem address.
    size_t nextGeneratedAddressNum = 0;

    /// Memory table for keeping track of the size allocated.
    std::unordered_map<uint64_t, size_t> _MemTable;

    /// Reference name table.
    std::unordered_map<uint64_t, std::string> _RefTable;

    /// Reserved memory blocks available.
    std::vector<ReservedMemBlock> _ReservedBlocks;

    //! Constructor.
    Alloc() {}

    //! Generates a unique memory address.
    template<class T>
    uint64_t GenerateMemAddress(T* ref)
    {
//     auto guid = xg::newGuid();
//     std::stringstream guidStream;
//     guidStream << guid;
//     return guidStream.str();

    //  return GenerateRandomStr(32); //fmt::format("{0}", nextGeneratedAddressNum++);

        // https://stackoverflow.com/questions/8287188/stdostringstream-printing-the-address-of-the-c-string-instead-of-its-content
        // return dynamic_cast<std::ostringstream&>(
        //         std::ostringstream().flush() << ref
        // ).str();

        // return reinterpret_cast<uint64_t>(&ref);
        return atol(GenerateRandomStr(17, true).c_str());
        //  return nextGeneratedAddressNum++;
    }

    //! Attempt reserved blocks of memory.
    //! \param amount
    //! \param data
    bool _ReserveBlocks();

    //! Attempt to free reserved blocks of memory.
    //! \param amount
    //! \param data
    void _FreeBlocks();

    //! Attempt to free a reserved block of memory.
    //! \param index
    void _FreeBlock(long index);

    //! Attempt to find a free reserved block of memory.
    //! \param amount
    //! \param data
    bool _TryFindFreeReserveBlock(size_t amount, void*& data);

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
            _Instance->_FreeBlocks();
            delete _Instance;
        }
    }

    // https://stackoverflow.com/questions/440133/how-do-i-create-a-random-alpha-numeric-string-in-c
    // given a function that generates a random character,
    // return a string of the requested length
    inline std::string GenerateRandomStr(size_t length, bool numbersOnly = false)
    {
        auto randchar = []() -> char
        {
            const char an_charset[] =
                    "0123456789"
                    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                    "abcdefghijklmnopqrstuvwxyz";
            const short an_max_index = (sizeof(an_charset) - 1);
            return an_charset[ rand() % an_max_index ];
        };

        auto n_randchar = []() -> char
        {
            const char n_charset[] =
                    "0123456789";
            const short n_max_index = (sizeof(n_charset) - 1);

            return n_charset[ rand() % n_max_index ];
        };

        std::string str(length, 0);

        if (numbersOnly) {
            std::generate_n( str.begin(), length, n_randchar );
        } else {
            std::generate_n( str.begin(), length, randchar );
        }

        return str;
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

        if (!_TryFindFreeReserveBlock(size, ref->_Data)) {
            ref->_Data = malloc(size);
        }

        ref->_MemAddress = GenerateMemAddress<T>(ref);
        ref->_Guid = GenerateRandomStr(5);

        AddMemAllocated(sizeof(T) + ref->_MemSize);

        auto entry = _MemTable.find(sympl_address_ref(ref));
        assert(entry == _MemTable.end() && "Invalid allocation to memory address (duplicate insert)!");

        _MemTable[sympl_address_ref(ref)] = sizeof(T) + ref->_MemSize;
        _RefTable[sympl_address_ref(ref)] = ref->GetTypeName();

        return ref;
    }

    //! Free memory from a sympl reference.
    template<class T>
    inline void FreeRef(T*& ref)
    {
        auto address_ref = sympl_address_ref(ref);

        // If we don't have an address ref we probably didn't allocate memory (static variable).
        // (i.e. ScriptContext::Empty)
        if (ref->Guid().size() == 0) {
            ref->Free();
            return;
        }

        auto entry = _MemTable.find(address_ref);
        assert(entry != _MemTable.end() && "Invalid access to free memory!");

        if (entry == _MemTable.end()) {
            return;
        }

        if (!ref->Free()) {
            return;
        }

        size_t size = entry->second;
        RemoveMemAllocated(size);
        _MemTable.erase(address_ref);
        _RefTable.erase(address_ref);

        free(ref->_Data);
        delete ref;
        ref = nullptr;
    }

    //! Returns a string of the references still in memory.
    //! \return string
    inline std::string PrintRefs() {
        std::string memAddresses;
        for (auto entryIt : _MemTable) {
            memAddresses.append(std::to_string(entryIt.first));
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
