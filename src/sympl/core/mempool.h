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
        size_t  Index;
        bool    Free;
        void*   Data;
    };

    /// Memory pool of allocated space with padding.
    void* _Pool = nullptr;

    /// Blocks in the pool.
    std::vector<MemBlock*> _Blocks;

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
    void ClearBlock(size_t index);

    //! Fills the block with junk data.
    //! \param index
    void FillBlock(size_t index);
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
        ref->SetMemIndex(block->Index);

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
        if (ref->GetMemIndex() == -1) {
            return;
        }

        size_t blockIndex = ref->GetMemIndex();

        ref->Release();
        ref->~T();

        _Blocks[blockIndex]->Free = true;
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
        ref->~T();
        block->Free = true;
    }
};

class MemPoolManager final
{
private:
    // Pools available in the manager.
    std::vector<MemPool*> _Pools;

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

    //! Creates a new pool.
    //! \tparam T
    //! \tparam Padding
    //! \tparam Size
    //! \param isRefCounter
    //! \param maxMemBlocks
    template<class T, size_t Padding = 64, size_t Size = sizeof(T)>
    MemPool* CreatePool(bool isRefCounter = true, size_t maxMemBlocks = 200)
    {
        MemPool* pool = nullptr;

        if (isRefCounter) {
            pool = new MemPoolRef(T::GetTypeNameStatic().c_str(), Size, isRefCounter, Padding, maxMemBlocks);
        } else {
            pool = new MemPoolObject(T::GetTypeNameStatic().c_str(), Size, isRefCounter, Padding, maxMemBlocks);
        }
        _Pools.push_back(pool);

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
        MemPool* pool = nullptr;

        if (isRefCounter) {
            pool = new MemPoolRef(typeName, Size, isRefCounter, Padding, maxMemBlocks);
        } else {
            pool = new MemPoolObject(typeName, Size, isRefCounter, Padding, maxMemBlocks);
        }
        _Pools.push_back(pool);

        return pool;
    };

    //! Attempts to find and return a memory pool.
    //! \param typeName
    //! \return MemPool
    MemPool* FindPool(const char* typeName)
    {
        for (unsigned i = 0; i < _Pools.size(); i++) {
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
        FindOrAlloc<T>(typeName.c_str());
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
        return new T();
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
        auto pool = FindPool(ref->GetTypeName().c_str());
        if (!IsNullObject(pool)) {
            ((MemPoolRef*)pool)->template Deallocate<T>(ref);
            return;
        }

        if (ref->DecRef()) {
            return;
        }
        ref->Release();
        delete ref;
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
    }
};

#define MemPoolInstance MemPoolManager::GetInstance()
#define mem_create_pool(type, amount) MemPoolInstance.CreatePool<type>(true, amount)
#define mem_create_object_pool(type, amount) MemPoolInstance.CreatePool<type>(#type, false, amount)
#define mem_alloc_ref(type) MemPoolInstance.AllocateRef<type>(type::GetTypeNameStatic())
#define mem_free_ref(type, ref) MemPoolInstance.DeallocateRef<type>(ref)
#define mem_alloc_object(type) MemPoolInstance.AllocateObject<type>(#type)
#define mem_free_object(type, ref) MemPoolInstance.DeallocateObject<type>(#type, ref)

sympl_nsend
