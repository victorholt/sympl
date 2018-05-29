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

class ObjectRef;

// Representation of a block address.
typedef long long BlockAddr;

class AllocManager
{
private:
    // Representation of a block of pool memory.
    struct MemBlock
    {
        /// Flag for whether or not this block is free.
        bool        IsFree;
        /// Size of the block.
        size_t      Size;
        /// Starting index of this block in the pool memory.
        size_t      Index;
        /// Reference to the data contained in the block.
        void*       Data;
    };

    /// Memory pool list for holding 'ObjectRef' objects.
    std::vector<MemBlock>   _ObjectPoolList;
    /// Memory pool list for holding bytes for strings/non-objects.
    std::vector<MemBlock>   _BytePoolList;

    /// Map of objects in memory that are not using the pool.
    std::unordered_map<BlockAddr, ObjectRef*> _ReferenceMap;

    /// Size of a single memory block for the object pool.
    size_t _ObjectPoolBlockSize     = 1048;
    /// Size of a single memory block for the byte pool.
    size_t _BytePoolBlockSize       = 256;
    /// Max number of blocks that can be created in the object pool.
    size_t _MaxObjectPoolCount      = 50000;
    /// Max number of blocks that can be created in the byte pool.
    size_t _MaxBytePoolCount        = 25000;
    /// Max allowed memory allocation.
    size_t _MaxAllowedMemoryAlloc   = 2048000000;

    /// Reference to the object memory pool.
    void* _ObjectMemPool;
    /// Reference to the bytes memory pool.
    void* _ByteMemPool;

    /// AllocManager singleton instance.
    static AllocManager* _Instance;

    //! Constructor.
    AllocManager();

    //! Initializes the allocation manager.
    void _Initialize();

    //! Copy over the object block data to the new pool.
    void _CopyObjectBlockData(void* newPool);

    //! Copy over the byte block data to the new pool.
    void _CopyByteBlockData(void* newPool);

    //! Returns the index of an available object block.
    MemBlock* _FindAvailableObjectBlock();

    //! Returns the index of an available byte block.
    MemBlock* _FindAvailableByteBlock();

public:
    //! Returns the singleton for our Alloc class.
    //! \return Alloc*
    inline static AllocManager* GetInstance() {
        if (IsNullObject(_Instance)) {
            _Instance = new AllocManager();
        }
        return _Instance;
    }

    //! Attempts to allocate memory for an object reference.
    //! \param ref
    //! \param amount
    template<typename T>
    T* AllocRef()
    {
        // Attempt to find an available block in memory.
        AllocManager::MemBlock* block = _FindAvailableObjectBlock();

        // Ensure we can allocate the bytes.
        assert(sizeof(T) < _ObjectPoolBlockSize && "Unable to allocate the given amount of memory");

        // If we don't have an available block, try to allocate more space.
        if (IsNullObject(block)) {
            assert(
                    (_MaxObjectPoolCount + _MaxObjectPoolCount) < _MaxAllowedMemoryAlloc
                    && "Failed to allocate from the object memory pool!"
            );

            ResizeObjectPoolList(_MaxObjectPoolCount + _MaxObjectPoolCount, _ObjectPoolBlockSize);
            block = _FindAvailableObjectBlock();

            assert(!IsNullObject(block) && "Failed to allocate object memory block!");
        }

        memset(_ObjectMemPool + (block->Index * _ObjectPoolBlockSize), 0, _ObjectPoolBlockSize);
        block->IsFree = false;
        block->Size = sizeof(T);

//    T* ref = reinterpret_cast<T*>(block->Data);
        T* ref = new(block->Data) T();
        ref->SetMemIndex(block->Index);

        return ref;
    }

    //! Attempts to allocate memory.
    //! \param amount
    //! \return void*
    void* AllocBytes(size_t amount);

    //! Attempts to free an object reference.
    //! \param T
    //! \param ref
    template<typename T>
    void FreeRef(T*& ref)
    {
        auto memIndex = ref->GetMemIndex();
        assert(memIndex < _ObjectPoolList.size());

        // Check to ensure we're not trying to free static memory.
        if (memIndex == 0) {
            void* dataCheck = reinterpret_cast<void*>(ref);
            if (_ObjectPoolList[0].Data != dataCheck) {
                return;
            }
        }

        _ObjectPoolList[memIndex].Size = 0;
        _ObjectPoolList[memIndex].IsFree = true;

        ref->~T();
        ref = nullptr;
    }

    //! Attempts to free bytes.
    //! \param src
    void FreeBytes(void* src);

    //! Resize the object pool list.
    //! \param count
    //! \param blockSize
    void ResizeObjectPoolList(size_t count, size_t blockSize);

    //! Resize the byte pool list.
    //! \param count
    //! \param blockSize
    void ResizeBytePoolList(size_t count, size_t blockSize);

    //! Returns the current memory usage.
    //! \return size_t
    size_t GetMemoryUsage() const;

    //! Returns the unused memory usage.
    //! \return size_t
    size_t GetUnusedMemory() const;

    //! Returns the available object blocks.
    //! \return size_t
    size_t GetAvailableObjectBlocks() const;

    //! Returns the available byte blocks.
    //! \return
    size_t GetAvailableByteBlocks() const;

    //! Returns the total available blocks.
    //! \return
    size_t GetTotalAvailableBlocks();

    //! Sets the max allowed memory allocation.
    //! \param amount
    inline void SetMaxAllowedMemoryAlloc(size_t amount) {
        if (amount <= _MaxAllowedMemoryAlloc) return;
        _MaxAllowedMemoryAlloc = amount;
    }
};

#define alloc_ref(clazz) Sympl::AllocManager::GetInstance()->AllocRef<clazz>()
#define free_ref(type, ref) Sympl::AllocManager::GetInstance()->FreeRef<type>(ref)

#define alloc_bytes(amount) Sympl::AllocManager::GetInstance()->AllocBytes(amount)
#define free_bytes(ref) Sympl::AllocManager::GetInstance()->FreeBytes(reinterpret_cast<void*>(ref))

#define AllocInstance Sympl::AllocManager::GetInstance()

sympl_nsend
