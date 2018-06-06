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
#include <sympl/core/ref.h>

#include <fmt/format.h>
sympl_namespaces

void AllocReserveGroup::_Initialize(size_t totalBlocks, size_t blockSize)
{
    if (_MemPool == nullptr) {
//        _MemPool = calloc(totalBlocks * blockSize, sizeof(char));
//        _MemPool = malloc(totalBlocks * blockSize);
    }

    _BlockSize = blockSize;
    _BlocksUsed = 0;
    Resize(totalBlocks, blockSize);
}

void AllocReserveGroup::Resize(size_t count, size_t blockSize)
{
    if (count <= _Blocks.size() && !_Blocks.empty()) {
        return;
    }

    size_t start = _Blocks.empty() ? 0 : _Blocks.size();
    _Blocks.reserve(count);

    for (size_t i = start; i < count; i++) {
        auto block         = new AllocReserveGroup::MemBlock();
        block->Index        = i;
        block->IsFree       = true;
        block->Size         = 0;
        block->Data         = nullptr; // calloc(blockSize, sizeof(char));
//        memset(block->Data, 1, sizeof(char) * blockSize);
//        block.Data       = static_cast<char*>(_MemPool) + (i * blockSize);//malloc(blockSize * sizeof(char));//calloc(blockSize, sizeof(char));
//        memset(static_cast<char*>(_MemPool) + (block.Index * blockSize), 0, blockSize);

        _Blocks.push_back(block);
    }
}

void AllocReserveGroup::ClearBlockMemory(size_t index)
{
    auto block = _Blocks[index];
//    memset(static_cast<char*>(_MemPool) + (block->Index * block->Size), 0, block->Size);
//    memset(block->Data, 0, block->Size);
}

AllocReserveGroup::MemBlock* AllocReserveGroup::_FindAvailable()
{
    for (size_t i = 0; i < _Blocks.size(); i++) {
        auto block = _Blocks[i];
        if (block->IsFree) {
            return block;
        }
    }
    return nullptr;
}

AllocManager::AllocManager()
{
    _Initialize();
}

void AllocManager::_Initialize()
{
//    _ObjectList._Initialize(_MaxObjectPoolCount, _ObjectPoolBlockSize);
//    _ByteList._Initialize(_MaxBytePoolCount, _BytePoolBlockSize);
}

void AllocManager::FreeRef(RefCounter* ref)
{
//    delete ref;
}

void AllocManager::FreeBytes(void* src, bool isArray)
{
    if (isArray) {
        delete [] src;
    } else {
        delete src;
    }
}

void AllocManager::FreeBytesArray(void* src)
{
    FreeBytes(src, true);
}

size_t AllocManager::GetMemoryUsage() const
{
    size_t memUsage = 0;
    for (auto block : _ObjectList._Blocks) {
        if (!block->IsFree) {
             memUsage += _ObjectPoolBlockSize;
        }
    }

    for (auto block : _ByteList._Blocks) {
        if (!block->IsFree) {
            memUsage += _BytePoolBlockSize;
        }
    }

    return memUsage;
}

size_t AllocManager::GetUnusedMemory() const
{
    size_t memUsage = 0;
    for (auto block : _ObjectList._Blocks) {
        if (block->IsFree) {
            memUsage += _ObjectPoolBlockSize;
        }
    }

    for (auto block : _ByteList._Blocks) {
        if (block->IsFree) {
            memUsage += _BytePoolBlockSize;
        }
    }

    return memUsage;
}

size_t AllocManager::GetAvailableObjectBlocks() const
{
    size_t count = 0;
    for (auto block : _ObjectList._Blocks) {
        if (block->IsFree) {
            count++;
        }
    }
    return count;
}

size_t AllocManager::GetAvailableByteBlocks() const
{
    size_t count = 0;
    for (auto block : _ByteList._Blocks) {
        if (block->IsFree) {
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
    for (auto block : _ObjectList._Blocks) {
        if (block->IsFree) {
            continue;
        }

        auto ref = reinterpret_cast<RefCounter*>(block->Data);
        memAddresses.append(ref->GetTypeName());
        memAddresses.append("\n");
        count++;
    }
    memAddresses.append(fmt::format("{0} total references", count));
    return memAddresses;
}

