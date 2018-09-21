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
#include <sympl/core/mempool.h>
#include <sympl/core/ref.h>

#include <fmt/format.h>
sympl_namespaces

MemPool::MemPool(const char* typeName, size_t blockSize, bool isRefCounter, size_t padding, size_t maxMemBlocks)
{
    memset(_TypeName, 0, 256);
    memcpy(_TypeName, typeName, strlen(typeName));

    _IsRefCounter = isRefCounter;
    _BlockSize = blockSize;
    _Padding = padding;
    _MaxMemBlocks = 0;

    Resize(maxMemBlocks);
}

MemPool::~MemPool()
{
    for (auto& block : _Blocks) {
        free(block->Data);
        delete block;
    }
    _Blocks.clear();
//    free(_Pool);
}

MemPool::MemBlock* MemPool::FindAvailableBlock()
{
    // First Scan.
    auto size = _Blocks.size();
    for (unsigned i = _LastFreeIndex; i < size; i++) {
        if (_Blocks[i]->Free) {
            _LastFreeIndex = _Blocks[i]->Index + 1;
            return _Blocks[i];
        }
    }

    // Second scan from the beginning.
    _LastFreeIndex = 0;
    for (unsigned i = _LastFreeIndex; i < size; i++) {
        if (_Blocks[i]->Free) {
            _LastFreeIndex = _Blocks[i]->Index + 1;
            return _Blocks[i];
        }
    }

    // Allocate more memory.
    _LastFreeIndex = _MaxMemBlocks;
    size_t newAmount = _MaxMemBlocks * 2;
    Resize(newAmount);

    return FindAvailableBlock();

//    sympl_assert(false, "Unable to allocate memory block!");
//    return nullptr;
}

MemPool::MemBlock* MemPool::FindBlock(void* ref)
{
    auto size = _Blocks.size();
    for (unsigned i = 0; i < size; i++) {
        if (!_Blocks[i]->Free && _Blocks[i]->Data == ref) {
            return _Blocks[i];
        }
    }
    return nullptr;
}

void MemPool::ClearBlock(size_t index)
{
//    memset((char*)_Pool + (index * (_BlockSize + _Padding)), 0, _BlockSize + _Padding);
    memset(_Blocks[index]->Data, 0, _BlockSize + _Padding);
}

void MemPool::ClearAll()
{
    for (size_t i = 0; i < _MaxMemBlocks; i++) {
        ClearBlock(i);
    }
}

void MemPool::FillBlock(size_t index)
{
    memset(_Blocks[index]->Data, 0xDD, _BlockSize + _Padding);
}

void MemPool::Resize(size_t amount)
{
    size_t newSize = _MaxMemBlocks + amount;
    for (size_t i = _MaxMemBlocks; i < newSize; i++) {
        auto block = new MemBlock();
        block->Index = i;
        block->Free = true;
        block->Data = malloc(_BlockSize + _Padding);
        _Blocks.push_back(block);
        FillBlock(block->Index);
    }
    _MaxMemBlocks = newSize;
}

MemPoolRef::MemPoolRef(const char* typeName, size_t blockSize, bool isRefCounter, size_t padding, size_t maxMemBlocks)
    : MemPool(typeName, blockSize, isRefCounter, padding, maxMemBlocks)
{
}

MemPoolObject::MemPoolObject(const char* typeName, size_t blockSize, bool isRefCounter, size_t padding, size_t maxMemBlocks)
        : MemPool(typeName, blockSize, isRefCounter, padding, maxMemBlocks)
{
}

MemPoolBytes::MemPoolBytes(const char* typeName, size_t blockSize, bool isRefCounter, size_t padding, size_t maxMemBlocks)
        : MemPool(typeName, blockSize, isRefCounter, padding, maxMemBlocks)
{

}
