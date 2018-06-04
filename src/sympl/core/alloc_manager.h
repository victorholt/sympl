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

class AllocReserveGroup
{
private:
    // Representation of a block of pool memory.
    struct MemBlock
    {
        /// Flag for whether or not this block is free.
        bool        IsFree;
        /// Flag for if this block is dirty.
        bool        IsDirty;
        /// Size of the block.
        size_t      Size;
        /// Starting index of this block in the pool memory.
        size_t      Index;
        /// Reference to the data contained in the block.
        void*       Data;
    };

    /// Memory pool list for holding 'ObjectRef' objects.
    std::vector<MemBlock>   _Blocks;

    /// Memory pool.
    void* _MemPool = 0;

    /// Size of a single block.
    size_t _BlockSize;

    //! Initializes the allocation manager.
    void _Initialize(size_t totalBlocks, size_t blockSize);

    //! Returns the index of an available object block.
    MemBlock* _FindAvailable();

    //! Resize the list.
    //! \param count
    //! \param blockSize
    void Resize(size_t count, size_t blockSize);

public:
    friend class AllocManager;
};

class AllocManager
{
private:
    /// Alloc reserve group for objects.
    AllocReserveGroup _ObjectList;

    /// Alloc reserve group for objects.
    AllocReserveGroup _ByteList;

    /// Size of a single memory block for the object pool.
    size_t _ObjectPoolBlockSize     = 1048;
    /// Size of a single memory block for the byte pool.
    size_t _BytePoolBlockSize       = 256;
    /// Max number of blocks that can be created in the object pool.
    size_t _MaxObjectPoolCount      = 50000;
    /// Max number of blocks that can be created in the byte pool.
    size_t _MaxBytePoolCount        = 15000;
    /// Max allowed memory allocation.
    size_t _MaxAllowedMemoryAlloc   = 2048000000;

    /// AllocManager singleton instance.
    static AllocManager* _Instance;

    //! Constructor.
    AllocManager();

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
    template<typename T>
    T* AllocRef()
    {
        // Attempt to find an available block in memory.
        AllocReserveGroup::MemBlock* block = _ObjectList._FindAvailable();

        // Ensure we can allocate the bytes.
        assert(sizeof(T) < _ObjectPoolBlockSize && "Unable to allocate the given amount of memory");

        // If we don't have an available block, try to allocate more space.
        if (IsNullObject(block)) {
            assert(
                    (_MaxObjectPoolCount + _MaxObjectPoolCount) < _MaxAllowedMemoryAlloc
                    && "Failed to allocate from the object memory pool!"
            );

            ResizeObjectPoolList(_MaxObjectPoolCount + _MaxObjectPoolCount, _ObjectPoolBlockSize);
            block = _ObjectList._FindAvailable();

            assert(!IsNullObject(block) && "Failed to allocate object memory block!");
        }

        if (block->IsDirty) {
            memset(static_cast<char*>(_ObjectList._MemPool) + (block->Index * block->Size), 0, block->Size);
//            memset(block->Data, 0, block->Size);
        }

        block->IsFree = false;
        block->IsDirty = true;
        block->Size = sizeof(T);

//        T* ref = reinterpret_cast<T*>(block->Data);
        T* ref = new(block->Data) T();
        ref->SetMemIndex(block->Index);

        return ref;
    }

    //! Attempts to allocate memory.
    //! \param amount
    //! \return void*
    void* AllocBytes(size_t amount);

    //! Attempts to free an object reference.
    //! \param ref
    void FreeRef(ObjectRef* ref);

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
    //! \return size_t
    size_t GetAvailableByteBlocks() const;

    //! Returns the total available blocks.
    //! \return size_t
    size_t GetTotalAvailableBlocks();

    //! Returns a string of the existing references.
    //! \return string
    std::string PrintExistingReferences();

    //! Sets the max allowed memory allocation.
    //! \param amount
    inline void SetMaxAllowedMemoryAlloc(size_t amount) {
        if (amount <= _MaxAllowedMemoryAlloc) return;
        _MaxAllowedMemoryAlloc = amount;
    }
};

#define alloc_ref(clazz) Sympl::AllocManager::GetInstance()->AllocRef<clazz>()
#define free_ref(ref) Sympl::AllocManager::GetInstance()->FreeRef(reinterpret_cast<ObjectRef*>(ref))

#define alloc_bytes(amount) Sympl::AllocManager::GetInstance()->AllocBytes(amount)
#define free_bytes(ref) Sympl::AllocManager::GetInstance()->FreeBytes(reinterpret_cast<void*>(ref))

#define AllocInstance Sympl::AllocManager::GetInstance()

sympl_nsend
