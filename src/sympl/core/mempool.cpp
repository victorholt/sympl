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

#include <sympl/util/string_helper.h>

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
        MemPoolInstance.ReleaseObjectAddress(block.first);
        free(block.second->Data);
        delete block.second;
    }
    _Blocks.clear();

    _AvailableObjectAddresses.clear();
}

MemPool::MemBlock* MemPool::FindAvailableBlock()
{
    // Check if we need to resize the available addresses.
    if (_AvailableObjectAddresses.empty()) {
        Resize(_MaxMemBlocks * 2);
    }

    auto address = ReserveObjectAddress();
    return _Blocks[address];
}

MemPool::MemBlock* MemPool::FindBlock(void* ref)
{
    for (const auto& entry : _Blocks) {
        if (entry.second->Free && entry.second->Data == ref) {
            return entry.second;
        }
    }
    return nullptr;
}

void MemPool::ClearBlock(MemAddressType address)
{
    memset(_Blocks[address]->Data, 0, _BlockSize + _Padding);
}

void MemPool::ClearAll()
{
    for (const auto& entry : _Blocks) {
        ClearBlock(entry.first);
    }
}

void MemPool::FillBlock(MemAddressType address)
{
    memset(_Blocks[address]->Data, 0xDD, _BlockSize + _Padding);
}

void MemPool::Resize(size_t amount)
{
    size_t newSize = _MaxMemBlocks + amount;
    for (size_t i = _MaxMemBlocks; i < newSize; i++) {
        // Grab our address from the manager.
        auto address = MemPoolInstance.ReserveObjectAddress();

        // Push the address into available addresses.
        ReleaseObjectAddress(address);

        auto block = new MemBlock();
        block->Address = address;
        block->Free = true;
        block->Data = malloc(_BlockSize + _Padding);

        _Blocks[address] = block;
        FillBlock(address);
    }
    _MaxMemBlocks = newSize;
}

MemAddressType MemPool::ReserveObjectAddress()
{
    MemAddressType result = _AvailableObjectAddresses.back();
    _AvailableObjectAddresses.pop_back();
    return result;
}

void MemPool::ReleaseObjectAddress(MemAddressType address)
{
    _AvailableObjectAddresses.emplace_back(address);
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

void MemPoolManager::_BuildAddresses()
{
    _AvailableObjectAddresses.clear();
    _AvailableObjectAddresses.reserve(_MaxObjectAddresses);

    for (size_t i = 0; i < _MaxObjectAddresses; i++) {
//        auto address = StringHelper::GenerateRandomStr(64);
        _AvailableObjectAddresses.emplace_back(i);
    }
}

void MemPoolManager::Initialize()
{
    if (_Init) {
        return;
    }

    if (!_Init) {
        _BuildAddresses();
    }
    _Init = true;
}

MemAddressType MemPoolManager::ReserveObjectAddress()
{
    MemAddressType result = _AvailableObjectAddresses.back();
    _AvailableObjectAddresses.pop_back();
    return result;
}

void MemPoolManager::ReleaseObjectAddress(MemAddressType address)
{
//    sympl_assert(IsAvailableObjectAddress(address), "Attempted to release an object address that's already available!");
    _AvailableObjectAddresses.emplace_back(address);
}

bool MemPoolManager::IsAvailableObjectAddress(MemAddressType address)
{
    for (auto entry : _AvailableObjectAddresses) {
        if (entry == address) {
            return false;
        }
    }

    return true;
}
