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

#include <fmt/format.h>
sympl_namespaces

AllocManager* AllocManager::_Instance = nullptr;

void AllocReserveGroup::_Initialize(size_t totalBlocks, size_t blockSize)
{
    if (_MemPool == 0) {
        _MemPool = calloc(totalBlocks * blockSize, sizeof(char));
    }
    Resize(totalBlocks, blockSize);
}

void AllocReserveGroup::Resize(size_t count, size_t blockSize)
{
    if (count <= _Blocks.size() && !_Blocks.empty()) {
        return;
    }

    size_t start = _Blocks.empty() ? 0 : _Blocks.size();

    _Blocks.reserve(count);
    _BlockSize = blockSize;

    for (size_t i = start; i < count; i++) {
        AllocReserveGroup::MemBlock block{};
        block.Index      = i;
        block.IsFree     = true;
        block.IsDirty    = false;
        block.Size       = 0;
        block.Data       = static_cast<char*>(_MemPool) + (i * blockSize);//malloc(blockSize * sizeof(char));//calloc(blockSize, sizeof(char));
//        memset(block.Data, 0, blockSize * sizeof(char));
        _Blocks.push_back(block);
    }
}

AllocReserveGroup::MemBlock* AllocReserveGroup::_FindAvailable()
{
    for (auto& block : _Blocks) {
        if (block.IsFree) {
            return &block;
        }
    }
    return nullptr;
}

void* AllocManager::AllocBytes(size_t amount)
{
    // Attempt to find an available block in memory.
    AllocReserveGroup::MemBlock* block = _ByteList._FindAvailable();

    // Ensure we can allocate the bytes.
    assert(amount < _BytePoolBlockSize && "Unable to allocate the given amount of bytes");

    // If we don't have an available block, try to allocate more space.
    if (IsNullObject(block)) {
        assert(
                (_MaxBytePoolCount + _MaxBytePoolCount) < _MaxAllowedMemoryAlloc
                && "Failed to allocate from the byte memory pool!"
        );

        ResizeBytePoolList(_MaxBytePoolCount + _MaxBytePoolCount, _BytePoolBlockSize);
        block = _ByteList._FindAvailable();

        assert(!IsNullObject(block) && "Failed to allocate byte memory block!");
    }

    if (block->IsDirty) {
        memset(static_cast<char*>(_ObjectList._MemPool) + (block->Index * block->Size), 0, block->Size);
//        memset(block->Data, 0, block->Size);
    }

    block->IsFree = false;
    block->IsDirty = true;
    block->Size = amount;

    return block->Data;
}

void AllocManager::FreeRef(ObjectRef* ref)
{
    auto memIndex = ref->GetMemIndex();
    assert(memIndex < _ObjectList._Blocks.size());

    if (ref->DecRef()) {
        return;
    }

    // Check to ensure we're not trying to free static memory.
    if (memIndex == 0) {
        void* dataCheck = reinterpret_cast<void*>(ref);
        if (_ObjectList._Blocks[0].Data != dataCheck) {
            return;
        }
    }

    _ObjectList._Blocks[memIndex].Size = 0;
    _ObjectList._Blocks[memIndex].IsFree = true;

    ref->Release();
}

void AllocManager::FreeBytes(void* src)
{
    for (auto& block : _ByteList._Blocks) {
        if (block.Data == src) {
            block.IsFree = true;
            return;
        }
    }
}

AllocManager::AllocManager()
{
    _ObjectList._Initialize(_MaxObjectPoolCount, _ObjectPoolBlockSize);
    _ByteList._Initialize(_MaxBytePoolCount, _BytePoolBlockSize);
}

size_t AllocManager::GetMemoryUsage() const
{
    size_t memUsage = 0;
    for (auto& block : _ObjectList._Blocks) {
        if (!block.IsFree) {
             memUsage += _ObjectPoolBlockSize;
        }
    }

    for (auto& block : _ByteList._Blocks) {
        if (!block.IsFree) {
            memUsage += _BytePoolBlockSize;
        }
    }

    return memUsage;
}

size_t AllocManager::GetUnusedMemory() const
{
    size_t memUsage = 0;
    for (auto& block : _ObjectList._Blocks) {
        if (block.IsFree) {
            memUsage += _ObjectPoolBlockSize;
        }
    }

    for (auto& block : _ByteList._Blocks) {
        if (block.IsFree) {
            memUsage += _BytePoolBlockSize;
        }
    }

    return memUsage;
}

size_t AllocManager::GetAvailableObjectBlocks() const
{
    size_t count = 0;
    for (auto& block : _ObjectList._Blocks) {
        if (block.IsFree) {
            count++;
        }
    }
    return count;
}

size_t AllocManager::GetAvailableByteBlocks() const
{
    size_t count = 0;
    for (auto& block : _ByteList._Blocks) {
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

void AllocManager::ResizeObjectPoolList(size_t count, size_t blockSize)
{
    _ObjectList.Resize(count, blockSize);
}

void AllocManager::ResizeBytePoolList(size_t count, size_t blockSize)
{
    _ByteList.Resize(count, blockSize);
}

std::string AllocManager::PrintExistingReferences()
{
    std::string memAddresses;
    size_t count = 0;
    for (auto& block : _ObjectList._Blocks) {
        if (block.IsFree) {
            continue;
        }

        auto ref = reinterpret_cast<ObjectRef*>(block.Data);
        memAddresses.append(ref->GetTypeName());
        memAddresses.append("\n");
        count++;
    }
    memAddresses.append(fmt::format("{0} total references", count));
    return memAddresses;
}

