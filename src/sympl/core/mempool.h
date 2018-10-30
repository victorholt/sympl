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

class MemPool
{
protected:
    /// Blocks in the pool.
    struct MemBlock
    {
        bool            Free;
        MemAddressType  Address;
        void*           Data;
    };

    /// Blocks in the memory pool.
    tsl::hopscotch_map<
        MemAddressType,
        MemBlock*
    > _Blocks;

    /// Available addresses that have been reserved.
    std::vector<MemAddressType> _AvailableObjectAddresses;

    /// Type name of the pool.
    char _TypeName[256] = {};

    /// Max memory blocks to allocate.
    size_t _MaxMemBlocks = 2000;

    /// Max memory blocks to allocate.
    size_t _BlockSize = 0;

    /// Max memory blocks to allocate.
    size_t _Padding = 0;

    /// Whether or not this pool manages a ref counter object.
    bool _IsRefCounter = false;

    //! Finds an available block.
    //! \return MemBlock
    MemBlock* FindAvailableBlock();

    //! Finds an available block.
    //! \return MemBlock
    MemBlock* FindBlock(void* ref);

    //! Clears the block memory.
    //! \param index
    void ClearBlock(MemAddressType address);

    //! Clear all blocks.
    void ClearAll();

    //! Fills the block with junk data.
    //! \param index
    void FillBlock(MemAddressType address);

    //! Resizes the memory pool.
    //! \param amount
    void Resize(size_t amount);

    //! Reserves an object address.
    //! \return std::string
    MemAddressType ReserveObjectAddress();

    //! Release an object address.
    //! \param address
    void ReleaseObjectAddress(MemAddressType address);

public:
    //! Constructor.
    MemPool();

    //! Destructor.
    ~MemPool();

    //! Constructor.
    //! \param typeName
    //! \param blockSize
    //! \param isRefCounter
    //! \param padding
    //! \param maxMemBlocks
    MemPool(const char* typeName, size_t blockSize, bool isRefCounter, size_t padding, size_t maxMemBlocks);

    //! Allocate new memory.
    //! \return T
    template<class T>
    T* Allocate() { return nullptr; }

    //! Deallocate memory.
    //! \param ref
    template<class T>
    void Deallocate(T* ref) {}

    //! Returns the type name.
    //! \return const char*
    inline const char* GetTypeName() const { return _TypeName; }

    //! Returns whether or not a type name is equal.
    //! \param typeName
    //! \return bool
    inline bool TypeNameEquals(const char* typeName)
    {
        return (strcmp(_TypeName, typeName) == 0);
    }

    //! Returns whether or not a type name is equal.
    //! \param typeName
    //! \return bool
    inline bool TypeNameEquals(const std::string& typeName)
    {
        return TypeNameEquals(typeName.c_str());
    }

    //! Returns whether or not this pool holds ref counter objects.
    //! \return bool
    inline bool IsRefCounter() const { return _IsRefCounter; }
};

class MemPoolRef : public MemPool
{
public:
    //! Constructor.
    //! \param typeName
    //! \param blockSize
    //! \param isRefCounter
    //! \param padding
    //! \param maxMemBlocks
    MemPoolRef(const char* typeName, size_t blockSize, bool isRefCounter, size_t padding, size_t maxMemBlocks);

    //! Allocate new memory.
    //! \return T
    template<class T>
    T* Allocate()
    {
        auto block = FindAvailableBlock();
        block->Free = false;

        T* ref = new(block->Data) T();
        ref->SetObjectAddress(block->Address);

        return ref;
    }

    //! Deallocate memory.
    //! \param ref
    template<class T>
    void Deallocate(T* ref)
    {
        if (ref->DecRef()) {
            return;
        }
        if (ref->IsStaticRef()) {
            return;
        }

        _Blocks[ref->GetObjectAddress()]->Free = true;
        ReleaseObjectAddress(ref->GetObjectAddress());

        ref->Release();
        ref->~T();
    }
};

class MemPoolObject : public MemPool
{
public:
    //! Constructor.
    //! \param typeName
    //! \param blockSize
    //! \param isRefCounter
    //! \param padding
    //! \param maxMemBlocks
    MemPoolObject(const char* typeName, size_t blockSize, bool isRefCounter, size_t padding, size_t maxMemBlocks);

    //! Allocate new memory.
    //! \return T
    template<class T>
    T* Allocate()
    {
        auto block = FindAvailableBlock();
        block->Free = false;

        T* ref = new(block->Data) T();
        return ref;
    }

    //! Deallocate memory.
    //! \param ref
    template<class T>
    void Deallocate(T* ref)
    {
        auto block = FindBlock(reinterpret_cast<void*>(ref));
        ReleaseObjectAddress(block->Address);
        ref->~T();
        block->Free = true;
    }
};

// Dynamic byte pool.
class MemPoolBytes : public MemPool
{
public:
    //! Constructor.
    //! \param typeName
    //! \param blockSize
    //! \param isRefCounter
    //! \param padding
    //! \param maxMemBlocks
    MemPoolBytes(const char* typeName, size_t blockSize, bool isRefCounter, size_t padding, size_t maxMemBlocks);

    //! Allocate new memory.
    //! \return T
    template<class T>
    T* Allocate()
    {
        auto block = FindAvailableBlock();
        block->Free = false;
        return block->Data;
    }

    //! Deallocate memory.
    //! \param ref
    template<class T>
    void Deallocate(T* ref)
    {
        auto block = FindBlock(reinterpret_cast<void*>(ref));
        ReleaseObjectAddress(block->Address);
        block->Free = true;
    }
};

class MemPoolManager final
{
private:
    /// Pools available in the manager.
    std::vector<MemPool*> _Pools;

    /// Max available addresses.
    size_t _MaxObjectAddresses = 100000;

    /// Memory usage size.
    size_t _MemoryUsage = 0;

    /// Flag to track memory allocations.
    bool _Debug = false;

    /// Flag to track whether or not the manager has been initialized.
    bool _Init = false;

    /// Memory allocation map (used when debug = true).
    std::unordered_map<std::string, long long int> _MemoryMap;

    /// Available address list for the pool.
    std::vector<MemAddressType> _AvailableObjectAddresses;

    //! Build the available object addresses.
    void _BuildAddresses();

    //! Constructor.
    explicit MemPoolManager() = default;

public:
    MemPoolManager(MemPoolManager const&)    = delete;
    void operator=(MemPoolManager const&)  = delete;

    //! Destructor.
    ~MemPoolManager() {
        for (auto pool : _Pools) {
            delete pool;
        }
        _Pools.clear();
    }

    //! Returns the singleton for our class.
    //! \return MemPoolManager*
    static MemPoolManager& GetInstance() {
        static MemPoolManager instance;
        return instance;
    }

    //! Initializes the pool manager.
    void Initialize();

    //! Creates a new pool.
    //! \tparam T
    //! \tparam Padding
    //! \tparam Size
    //! \param isRefCounter
    //! \param maxMemBlocks
    template<class T, size_t Padding = 64, size_t Size = sizeof(T)>
    MemPool* CreatePool(bool isRefCounter = true, size_t maxMemBlocks = 200)
    {
        Initialize();

        MemPool* pool = nullptr;

        if (isRefCounter) {
            pool = new MemPoolRef(T::GetTypeNameStatic().c_str(), Size, isRefCounter, Padding, maxMemBlocks);
        } else {
            pool = new MemPoolObject(T::GetTypeNameStatic().c_str(), Size, isRefCounter, Padding, maxMemBlocks);
        }
        _Pools.emplace_back(pool);

        _MemoryUsage += sizeof(T) * maxMemBlocks;

        UpdateMemoryMap(T::GetTypeNameStatic(), static_cast<int>(maxMemBlocks));

        return pool;
    };

    //! Creates a new pool.
    //! \tparam T
    //! \tparam Padding
    //! \tparam Size
    //! \param isRefCounter
    //! \param maxMemBlocks
    template<class T, size_t Padding = 64, size_t Size = sizeof(T)>
    MemPool* CreatePool(const char* typeName, bool isRefCounter = true, size_t maxMemBlocks = 200)
    {
        Initialize();

        MemPool* pool = nullptr;

        if (isRefCounter) {
            pool = new MemPoolRef(typeName, Size, isRefCounter, Padding, maxMemBlocks);
        } else {
            pool = new MemPoolObject(typeName, Size, isRefCounter, Padding, maxMemBlocks);
        }
        _Pools.emplace_back(pool);

        _MemoryUsage += sizeof(T) * maxMemBlocks;

        UpdateMemoryMap(T::GetTypeNameStatic(), static_cast<int>(maxMemBlocks));

        return pool;
    };

    //! Attempts to find and return a memory pool.
    //! \param typeName
    //! \return MemPool
    MemPool* FindPool(const char* typeName)
    {
        auto size = _Pools.size();
        for (unsigned i = 0; i < size; i++) {
            if (_Pools[i]->TypeNameEquals(typeName)) {
                return _Pools[i];
            }
        }
        return nullptr;
    }

    //! Attempts to find and return a memory pool.
    //! \param typeName
    //! \return MemPool
    MemPool* FindPool(const std::string& typeName)
    {
        return FindPool(typeName.c_str());
    }

    //! Attempts to find and return a memory pool.
    //! \param typeName
    //! \return MemPool
    template<class T>
    MemPool* FindOrCreatePool(const char* typeName)
    {
        auto pool = FindPool(typeName);
        if (!IsNullObject(pool)) return pool;

        return CreatePool<T>();
    }

    //! Attempts to find and return a memory pool.
    //! \param typeName
    //! \return MemPool
    template<class T>
    MemPool* FindOrCreatePool(const std::string& typeName)
    {
        return FindOrCreatePool<T>(typeName.c_str());
    }

    //! Attempts to find and return a memory pool.
    //! \param typeName
    //! \return MemPool
    template<class T>
    MemPool* FindOrAlloc(const std::string& typeName)
    {
        return FindOrAlloc<T>(typeName.c_str());
    }

    //! Attempts to find and return a memory pool.
    //! \param typeName
    //! \return MemPool
    template<class T>
    T* AllocateRef(const char* typeName)
    {
        auto pool = FindPool(typeName);
        if (!IsNullObject(pool)) {
            return ((MemPoolRef*)pool)->template Allocate<T>();
        }

        UpdateMemoryMap(T::GetTypeNameStatic(), 1);

        _MemoryUsage += sizeof(T);

        auto ref = new T();
        ref->SetObjectAddress(ReserveObjectAddress());

        return ref;
    }

    //! Attempts to find and return a memory pool.
    //! \param typeName
    //! \return MemPool
    template<class T>
    T* AllocateObject(const char* typeName)
    {
        auto pool = FindPool(typeName);
        if (!IsNullObject(pool)) {
            return ((MemPoolObject*)pool)->template Allocate<T>();
        }

        UpdateMemoryMap(typeName, 1);

        _MemoryUsage += sizeof(T);
        return new T();
    }

    //! Attempts to find and return a memory pool.
    //! \param typeName
    //! \return MemPool
    template<class T>
    T* AllocateRef(const std::string& typeName)
    {
        return AllocateRef<T>(typeName.c_str());
    }

    //! Attempts to find and return a memory pool.
    //! \param typeName
    //! \return MemPool
    template<class T>
    T* AllocateObject(const std::string& typeName)
    {
        return AllocateObject<T>(typeName.c_str());
    }

    //! Attempts to deallocate a reference.
    //! \tparam T
    //! \param ref
    template<class T>
    void DeallocateRef(T* ref)
    {
        if (IsNullObject(ref) || ref->IsStaticRef()) {
            return;
        }

        auto pool = FindPool(ref->GetTypeName().c_str());
        if (!IsNullObject(pool)) {
            ((MemPoolRef*)pool)->template Deallocate<T>(ref);
            return;
        }

//        if (ref->DecRef() || ref->GetMemIndex() < 0) {
//            return;
//        }

        if (ref->DecRef()) {
            return;
        }

        UpdateMemoryMap(ref->GetTypeName(), -1);
        ReleaseObjectAddress(ref->GetObjectAddress());

        ref->Release();
        delete ref;

        _MemoryUsage -= sizeof(T);
    }

    //! Attempts to deallocate a reference.
    //! \tparam T
    //! \param ref
    template<class T>
    void DeallocateObject(const char* typeName, T* ref)
    {
        auto pool = FindPool(typeName);
        if (!IsNullObject(pool)) {
            ((MemPoolObject*)pool)->template Deallocate<T>(ref);
            return;
        }
        delete ref;

        UpdateMemoryMap(typeName, -1);

        _MemoryUsage -= sizeof(T);
    }

    //! Updates the memory map with a given value.
    //! \param typeName
    //! \param value
    void UpdateMemoryMap(const std::string& typeName, int value)
    {
        if (!_Debug) {
            return;
        }

        auto entry = _MemoryMap.find(typeName);
        if (entry == _MemoryMap.end()) {
            _MemoryMap[typeName] = 0;
        }

        _MemoryMap[typeName] += value;
    }

    //! Reserves an object address.
    //! \return std::string
    MemAddressType ReserveObjectAddress();

    //! Releases an object address.
    //! \param address
    void ReleaseObjectAddress(MemAddressType address);

    //! Checks if an address is available.
    //! \param address
    //! \return bool
    bool IsAvailableObjectAddress(MemAddressType address);

    //! Sets the max number object addresses.
    //! \param value
    inline void SetMaxObjectAddresses(size_t value) { _MaxObjectAddresses = value; }

    //! Returns the max number object addresses.
    //! \return
    inline size_t GetMaxObjectAddresses() const { return _MaxObjectAddresses; }

    //! Prints the memory map.
    inline const std::unordered_map<std::string, long long int>& GetMemoryMap() { return _MemoryMap; }

    //! Returns the memory usage.
    //! \return size_t
    inline size_t GetMemoryUsage() const { return _MemoryUsage; }

    //! Sets the debug flag.
    //! \param value
    inline void SetDebug(bool value) { _Debug = value; }
};

#define MemPoolInstance MemPoolManager::GetInstance()
#define mem_create_pool(type, amount) MemPoolInstance.CreatePool<type>(true, amount)
#define mem_create_object_pool(type, amount) MemPoolInstance.CreatePool<type>(#type, false, amount)
#define mem_alloc_ref(type) MemPoolInstance.AllocateRef<type>(type::GetTypeNameStatic())
#define mem_free_ref(type, ref) MemPoolInstance.DeallocateRef<type>(ref)
#define mem_alloc_object(type) MemPoolInstance.AllocateObject<type>(#type)
#define mem_free_object(type, ref) MemPoolInstance.DeallocateObject<type>(#type, ref)

#define alloc_bytes(type) new type()
#define alloc_bytes_array(type, amount) (type*)calloc(amount, sizeof(type))
#define free_bytes(ref) delete ref; ref = nullptr
#define free_bytes_array(ref) free(ref); ref = nullptr

sympl_nsend
