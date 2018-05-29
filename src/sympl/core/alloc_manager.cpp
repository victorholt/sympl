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
#include <sympl/core/alloc_manager.h>
#include <sympl/core/object_ref.h>
sympl_namespaces

AllocManager* AllocManager::_Instance = nullptr;

AllocManager::AllocManager()
{
    _Initialize();
}

void AllocManager::_Initialize()
{
    _ObjectMemPool = calloc(_MaxObjectPoolCount * _ObjectPoolBlockSize, sizeof(char));
    _ByteMemPool = calloc(_MaxBytePoolCount * _BytePoolBlockSize, sizeof(char));

    _ObjectPoolList.reserve(_MaxObjectPoolCount);
    _BytePoolList.reserve(_MaxBytePoolCount);

    ResizeObjectPoolList(_MaxObjectPoolCount, _ObjectPoolBlockSize);
    ResizeBytePoolList(_MaxBytePoolCount, _BytePoolBlockSize);
}

void* AllocManager::AllocBytes(size_t amount)
{
    // Attempt to find an available block in memory.
    AllocManager::MemBlock* block = _FindAvailableByteBlock();

    // Ensure we can allocate the bytes.
    assert(amount < _BytePoolBlockSize && "Unable to allocate the given amount of bytes");

    // If we don't have an available block, try to allocate more space.
    if (IsNullObject(block)) {
        assert(
                (_MaxBytePoolCount + _MaxBytePoolCount) < _MaxAllowedMemoryAlloc
                && "Failed to allocate from the byte memory pool!"
        );

        ResizeBytePoolList(_MaxBytePoolCount + _MaxBytePoolCount, _BytePoolBlockSize);
        block = _FindAvailableByteBlock();

        assert(!IsNullObject(block) && "Failed to allocate byte memory block!");
    }

    memset(_ByteMemPool + (block->Index * _BytePoolBlockSize), 0, _BytePoolBlockSize);
    block->IsFree = false;
    block->Size = amount;

    return block->Data;
}

void AllocManager::FreeBytes(void* src)
{
    for (auto& block : _BytePoolList) {
        if (block.Data == src) {
            block.IsFree = true;
            return;
        }
    }
}

void AllocManager::ResizeObjectPoolList(size_t count, size_t blockSize)
{
    size_t oldPoolSize = _MaxObjectPoolCount * _ObjectPoolBlockSize;
    size_t newPoolSize = count * blockSize;

    // Check if we need to delete/update the old memory pool.
    if (newPoolSize <= oldPoolSize) {
        return;
    }

    _MaxObjectPoolCount = count;
    _ObjectPoolBlockSize = blockSize;

    void* oldPool = _ObjectMemPool;
    _ObjectMemPool = malloc(newPoolSize);

    _CopyObjectBlockData(_ObjectMemPool);

    free(oldPool);
}

void AllocManager::ResizeBytePoolList(size_t count, size_t blockSize)
{
    size_t oldPoolSize = _MaxBytePoolCount * _BytePoolBlockSize;
    size_t newPoolSize = count * blockSize;

    // Check if we need to delete/update the old memory pool.
    if (newPoolSize <= oldPoolSize) {
        return;
    }

    _MaxBytePoolCount = count;
    _BytePoolBlockSize = blockSize;

    void* oldPool = _ByteMemPool;
    _ByteMemPool = malloc(newPoolSize);

    _CopyByteBlockData(_ByteMemPool);

    free(oldPool);
}

size_t AllocManager::GetMemoryUsage() const
{
    size_t memUsage = 0;
    for (auto& block : _ObjectPoolList) {
        if (!block.IsFree) {
             memUsage += _ObjectPoolBlockSize;
        }
    }

    for (auto& block : _BytePoolList) {
        if (!block.IsFree) {
            memUsage += _BytePoolBlockSize;
        }
    }

    return memUsage;
}

size_t AllocManager::GetUnusedMemory() const
{
    size_t memUsage = 0;
    for (auto& block : _ObjectPoolList) {
        if (block.IsFree) {
            memUsage += _ObjectPoolBlockSize;
        }
    }

    for (auto& block : _BytePoolList) {
        if (block.IsFree) {
            memUsage += _BytePoolBlockSize;
        }
    }

    return memUsage;
}

size_t AllocManager::GetAvailableObjectBlocks() const
{
    size_t count = 0;
    for (auto& block : _ObjectPoolList) {
        if (block.IsFree) {
            count++;
        }
    }
    return count;
}

size_t AllocManager::GetAvailableByteBlocks() const
{
    size_t count = 0;
    for (auto& block : _BytePoolList) {
        if (block.IsFree) {
            count++;
        }
    }
    return count;
}

size_t AllocManager::GetTotalAvailableBlocks()
{
    return GetAvailableObjectBlocks() + GetAvailableByteBlocks();
}

void AllocManager::_CopyObjectBlockData(void* newPool)
{
    // Check if we are just now initializing the pool list.
    if (_ObjectPoolList.empty()) {
        for (size_t i = 0; i < _MaxObjectPoolCount; i++) {
            MemBlock newBlock{};
            newBlock.Index      = i;
            newBlock.IsFree     = true;
            newBlock.Size       = 0;
            newBlock.Data       = _ObjectMemPool + (i * _ObjectPoolBlockSize);
            _ObjectPoolList.push_back(newBlock);
        }
        return;
    }

    _ObjectPoolList.reserve(_MaxObjectPoolCount);
    for (auto& block : _ObjectPoolList) {
        block.Data = memcpy(newPool + (block.Index * _ObjectPoolBlockSize), block.Data, _ObjectPoolBlockSize);
    }
}
void AllocManager::_CopyByteBlockData(void* newPool)
{
    // Check if we are just now initializing the pool list.
    if (_BytePoolList.empty()) {
        for (size_t i = 0; i < _MaxBytePoolCount; i++) {
            MemBlock newBlock{};
            newBlock.Index      = i;
            newBlock.IsFree     = true;
            newBlock.Size       = 0;
            newBlock.Data       = _ByteMemPool + (i * _BytePoolBlockSize);
            _BytePoolList.push_back(newBlock);
        }
        return;
    }

    _BytePoolList.reserve(_MaxBytePoolCount);
    for (auto& block : _BytePoolList) {
        block.Data = memcpy(newPool + (block.Index * _BytePoolBlockSize), block.Data, _BytePoolBlockSize);
    }
}

AllocManager::MemBlock* AllocManager::_FindAvailableObjectBlock()
{
    for (auto& block : _ObjectPoolList) {
        if (block.IsFree) {
            return &block;
        }
    }
    return nullptr;
}

AllocManager::MemBlock* AllocManager::_FindAvailableByteBlock()
{
    for (auto& block : _BytePoolList) {
        if (block.IsFree) {
            return &block;
        }
    }
    return nullptr;
}

