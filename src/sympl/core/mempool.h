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
private:
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

    /// The next available block.
    size_t _NextAvailableBlock = 0;

    //! Finds an available blcok.
    //! \return MemBlock
    MemBlock* FindAvailableBlock();

    //! Clears the block memory.
    //! \param index
    void ClearBlock(size_t index);
public:
    //! Constructor.
    MemPool();

    //! Destructor.
    ~MemPool();

    //! Constructor.
    //! \param typeName
    //! \param blockSize
    //! \param padding
    //! \param maxMemBlocks
    MemPool(const char* typeName, size_t blockSize, size_t padding, size_t maxMemBlocks);

    //! Allocate new memory.
    //! \return T
    template<class T>
    T* Allocate()
    {
        auto block = FindAvailableBlock();
        block->Free = false;

//        ClearBlock(block->Index);

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
        _Blocks[ref->GetMemIndex()]->Free = true;
        ref->Release();
        ref->~T();
    }

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

    //! Returns the singleton for our Alloc class.
    //! \return Alloc*
    static MemPoolManager& GetInstance() {
        static MemPoolManager instance;
        return instance;
    }

    //! Creates a new pool.
    //! \tparam T
    //! \tparam Padding
    //! \tparam Size
    //! \param maxMemBlocks
    template<class T, size_t Padding = 64, size_t Size = sizeof(T)>
    MemPool* CreatePool(size_t maxMemBlocks = 200)
    {
        auto pool = new MemPool(T::GetTypeNameStatic().c_str(), Size, Padding, maxMemBlocks);
        _Pools.push_back(pool);

        return pool;
    };

    //! Attempts to find and return a memory pool.
    //! \param typeName
    //! \return MemPool
    MemPool* FindPool(const char* typeName)
    {
        for (auto pool : _Pools) {
            if (pool->TypeNameEquals(typeName)) {
                return pool;
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
    T* Allocate(const char* typeName)
    {
        auto pool = FindPool(typeName);
        if (!IsNullObject(pool)) {
            return pool->template Allocate<T>();
        }
        return new T();
    }

    //! Attempts to find and return a memory pool.
    //! \param typeName
    //! \return MemPool
    template<class T>
    T* Allocate(const std::string& typeName)
    {
        return Allocate<T>(typeName.c_str());
    }

    //! Attempts to deallocate a reference.
    //! \tparam T
    //! \param ref
    template<class T>
    void Deallocate(T* ref)
    {
        auto pool = FindPool(ref->GetTypeName().c_str());
        if (!IsNullObject(pool)) {
            pool->template Deallocate<T>(ref);
            return;
        }

        if (ref->DecRef()) {
            return;
        }
        ref->Release();
        delete ref;
    }
};

#define MemPoolInstance MemPoolManager::GetInstance()
#define mem_alloc(type) MemPoolInstance.Allocate<type>(type::GetTypeNameStatic())
#define mem_free(type, ref) MemPoolInstance.Deallocate<type>(ref)

sympl_nsend