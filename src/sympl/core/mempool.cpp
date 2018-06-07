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

MemPool::MemPool(const char* typeName, size_t blockSize, size_t padding, size_t maxMemBlocks)
{
    memset(_TypeName, 0, 256);
    memcpy(_TypeName, typeName, strlen(typeName));

    _BlockSize = blockSize;
    _Padding = padding;
    _MaxMemBlocks = maxMemBlocks;

    // Create our pool.
//    _Pool = malloc((blockSize + padding) * maxMemBlocks);

    // Add our memory blocks.
    for (size_t i = 0; i < _MaxMemBlocks; i++) {
        MemBlock* block = new MemBlock();
        block->Index = i;
        block->Free = true;
//        block->Data = ((char*)_Pool) + (i * (_BlockSize + _Padding));
        block->Data = malloc(_BlockSize + _Padding);
        _Blocks.push_back(block);
    }
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
    if (_NextAvailableBlock < _Blocks.size()) {
        return _Blocks[_NextAvailableBlock++];
    }

    for (auto& block : _Blocks) {
        if (block->Free) {
            _NextAvailableBlock = block->Index;
            return block;
        }
    }
    assert(false && "Unable to allocate memory block!");
}

void MemPool::ClearBlock(size_t index)
{
//    memset((char*)_Pool + (index * (_BlockSize + _Padding)), 0, _BlockSize + _Padding);
    memset(_Blocks[index]->Data, 0, _BlockSize + _Padding);
}