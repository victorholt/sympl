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

#define block_addr long long
#define MAX_RESERVE_BLOCKS              5000
#define MAX_RESERVE_BLOCK_SIZE          2049
#define sympl_address_ref(ref) ref->_MemAddress

struct ReservedMemBlock
{
    bool        Free;
    long long   Address;
    block_addr  Size;
    void*       Data;
};

class SYMPL_API Alloc {
private:
    /// Alloc singleton instance.
    static Alloc* _Instance;

    /// Memory allocated by our alloc class.
    size_t _MemAllocated = 0;

    /// Next number for the new generated mem address.
    size_t nextAddressIndex = 0;

    /// Memory table for keeping track of the size allocated.
    std::unordered_map<block_addr, size_t> _MemTable;

    /// Reference name table.
    std::unordered_map<block_addr, std::string> _RefTable;

    /// Reserved memory blocks available.
    std::vector<ReservedMemBlock> _ReservedBlocks;

    //! Constructor.
    Alloc() {
        _ReservedBlocks.reserve(MAX_RESERVE_BLOCKS);
        _MemTable.reserve(20000);
        _RefTable.reserve(20000);
    }

    //! Generates a unique memory address.
    template<class T>
    block_addr GenerateMemAddress(T* ref)
    {
//        auto guid = xg::newGuid();
//        std::stringstream guidStream;
//        guidStream << guid;
//        return guidStream.str();

//        return GenerateRandomStr(32); //fmt::format("{0}", nextGeneratedAddressNum++);

//        https://stackoverflow.com/questions/8287188/stdostringstream-printing-the-address-of-the-c-string-instead-of-its-content
//        return dynamic_cast<std::ostringstream&>(
//             std::ostringstream().flush() << ref
//        ).str();

        return atoll(GenerateRandomStr(16, true).c_str());
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
    void _FreeBlock(block_addr index);

    //! Attempt to find a free reserved block of memory.
    //! \param amount
    //! \param data
    //! \param memAddress
    bool _TryAllocBlock(size_t amount, void*& data, block_addr& memAddress);

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
            _Instance->_ReserveBlocks();
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
        T* ref;

        void*       memData;
        block_addr  memAddress;

        if (!_TryAllocBlock(size, memData, memAddress)) {
            memData = malloc(size);
            ref = new(memData) T();
            ref->_MemData = memData;
            ref->_MemAddress = GenerateMemAddress<T>(ref);

            AddMemAllocated(sizeof(T) + ref->_MemSize);

            auto entry = _MemTable.find(sympl_address_ref(ref));
            if (entry != _MemTable.end()) {
                assert(false && "Invalid allocation to memory address (duplicate insert)!");
            }

            _MemTable[sympl_address_ref(ref)] = sizeof(T) + ref->_MemSize;
            _RefTable[sympl_address_ref(ref)] = ref->GetTypeName();
        } else {
            ref = new(memData) T();
            ref->_MemData = memData;
            ref->_MemAddress = memAddress;
        }

        ref->_MemSize = size;
        ref->_Guid = GenerateRandomStr(5);

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

        if (!ref->Free()) {
            return;
        }

        // Try to find this in the reserved blocks.
        if (ref->_MemAddress < MAX_RESERVE_BLOCKS) {
            ref->~T();
            _FreeBlock(ref->_MemAddress);
        } else {
            auto entry = _MemTable.find(address_ref);
            if (entry == _MemTable.end()) {
                assert(false && "Invalid access to free memory!");
            }

            size_t size = entry->second;
            RemoveMemAllocated(size);
            _MemTable.erase(address_ref);
            _RefTable.erase(address_ref);

            ref->~T();
            free(ref->_MemData);
        }
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
