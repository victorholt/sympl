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
#include <sympl/core/alloc.h>
#include <sympl/core/sympl_ref.h>
sympl_namespaces

Alloc* Alloc::_Instance = nullptr;

bool Alloc::_ReserveBlocks()
{
    for (int i = 0; i < MAX_RESERVE_BLOCKS; i++) {
        ReservedMemBlock block;
        block.Data = malloc(MAX_RESERVE_BLOCK_SIZE);
        block.Size = MAX_RESERVE_BLOCK_SIZE;
        block.Free = true;
        block.Address = i;

        _ReservedBlocks.push_back(block);
//        AddMemAllocated(block.Size + sizeof(ReservedMemBlock));
    }
}

void Alloc::_FreeBlocks()
{
    for (int i = 0; i < MAX_RESERVE_BLOCKS; i++) {
        auto block = _ReservedBlocks[i];
        memset(block.Data, 0, block.Size);
        block.Free = true;
        RemoveMemAllocated(block.Size + sizeof(ReservedMemBlock));
    }
}

void Alloc::_FreeBlock(block_addr index)
{
    if (index < 0 || index >= MAX_RESERVE_BLOCKS) {
        return;
    }

    auto block = &_ReservedBlocks[index];
    memset(block->Data, 0, block->Size);
    RemoveMemAllocated(block->Size + sizeof(ReservedMemBlock));

    block->Free = true;
}

bool Alloc::_TryAllocBlock(size_t amount, void*& data, block_addr& memAddress)
{
    if (amount >= MAX_RESERVE_BLOCK_SIZE) return false;

    for (int i = 0; i < MAX_RESERVE_BLOCKS; i++) {
        if (_ReservedBlocks[i].Free) {
            data = _ReservedBlocks[i].Data;
            memAddress = i;
            _ReservedBlocks[i].Free = false;

            AddMemAllocated(_ReservedBlocks[i].Size + sizeof(ReservedMemBlock));
            return true;
        }
    }

    return false;
}


