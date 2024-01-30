// ================================================================================================
// -*- C++ -*-
// File: memory_pools.hpp
// Author: Guilherme R. Lampert
// Created on: 08/02/17
//
// About:
//  Useful templated memory pools of objects (header only).
//  This file provides a DynamicPool and a pair of FixedSizePools,
//  one of which has its size defined at compile time.
//
// Configuration switches:
//  MEM_POOL_DEBUG            0|1  -> Adds pool dumping and memory filling on alloc/free.
//  MEM_POOL_STATS            0|1  -> Adds stats tracking to the pools for debugging/profiling.
//  MEM_POOL_ASAN             0|1  -> Enables AddressSanitizer (Clang compiler only).
//  MEM_POOL_TEST_UNIT_TESTS  0|1  -> Enables some AddressSanitizer tests.
//
// License:
//  This software is in the public domain. Where that dedication is not recognized,
//  you are granted a perpetual, irrevocable license to copy, distribute, and modify
//  this file as you see fit. Source code is provided "as is", without warranty of any
//  kind, express or implied. No attribution is required, but a mention about the author
//  is appreciated.
//
// Source:
// https://gist.github.com/glampert/78aac2b371edcd57eb77a542d8fe5e46
//
// ================================================================================================

#ifndef MEMORY_POOLS_HPP
#define MEMORY_POOLS_HPP

#define MEM_POOL_DEBUG 1
#define MEM_POOL_STATS 1
#define MEM_POOL_ASAN  0
#define MEM_POOL_TEST_UNIT_TESTS 0

// ================================================================================================
//
//                              Configuration macros / Includes
//
// ================================================================================================

// C headers
#include <cstddef>
#include <cstdint>
#include <cstring>

// C++ headers
#include <iterator>
#include <memory>
#include <utility>

// typeid() used to print template types
#if MEM_POOL_DEBUG
#include <typeinfo>
    #include <cstdio>
#endif // MEM_POOL_DEBUG

// Easy way of stripping out code that is only for MEM_POOL_STATS
#if MEM_POOL_STATS
#define MEM_POOL_STATS_ONLY(...) __VA_ARGS__
#else //!MEM_POOL_STATS
#define MEM_POOL_STATS_ONLY(...) /* nothing */
#endif // MEM_POOL_STATS

// Configurable assert macro
#ifndef MEM_POOL_ASSERT
#include <cassert>
#define MEM_POOL_ASSERT assert
#endif // MEM_POOL_ASSERT

// Configurable base memory allocator
#ifndef MEM_POOL_ALLOC_IMPL
#include <new>
#define MEM_POOL_ALLOC_IMPL(type, count) new type[(count)]
#define MEM_POOL_FREE_IMPL(ptr, count)   delete[] (ptr)
#endif // MEM_POOL_ALLOC_IMPL

// Clang AddressSanitizer
#if defined(__clang__) && defined(__has_feature)
#if __has_feature(address_sanitizer)
#include <sanitizer/asan_interface.h>
        #define MEM_POOL_ASAN 1
#endif // address_sanitizer
#endif // __clang__ && __has_feature

// ================================================================================================
//
//                       Pool helpers / AddressSanitizer / Debug help
//
// ================================================================================================

namespace MemUtils
{

// ========================================================
// construct() / destroy() helpers:
// ========================================================

template<typename T>
T * construct(T * obj, const T & other) // Copy constructor
{
    return ::new(obj) T(other);
}

template<typename T, typename... Args>
T * construct(T * obj, Args &&... args) // Parameterized or default constructor
{
    return ::new(obj) T(std::forward<Args>(args)...);
}

template<typename T>
void destroy(T * obj) noexcept
{
    if (obj != nullptr)
    {
        obj->~T();
    }
}

// ========================================================
// Debug utilities:
// ========================================================

// Fill patterns for debug allocations:
constexpr std::uint8_t AllocFillVal = 0xCD; // 'Clean memory' -> New allocation
constexpr std::uint8_t FreeFillVal  = 0xDD; // 'Dead memory'  -> Freed/deleted

// Debug memset helpers:
#if MEM_POOL_DEBUG
inline void debugMemFill(void * ptr, const std::size_t sizeInBytes, const std::uint8_t fillVal) noexcept
{
    std::memset(ptr, fillVal, sizeInBytes);
}
#else // !MEM_POOL_DEBUG
inline void debugMemFill(void * /*ptr*/, std::size_t /*sizeInBytes*/, std::uint8_t /*fillVal*/) noexcept
{
    // No-op - should be optimized away.
}
#endif // MEM_POOL_DEBUG

// ========================================================
// AddressSanitizer:
// ========================================================

namespace ASan
{

#if MEM_POOL_ASAN

inline void poisonMemRegion(void const volatile * addr, const std::size_t sizeInBytes)
{
    ASAN_POISON_MEMORY_REGION(addr, sizeInBytes);
}
inline void unpoisonMemRegion(void const volatile * addr, const std::size_t sizeInBytes)
{
    ASAN_UNPOISON_MEMORY_REGION(addr, sizeInBytes);
}
inline bool addressIsPoisoned(void const volatile * addr)
{
    return __asan_address_is_poisoned(addr) != 0;
}
inline bool regionIsPoisoned(void * start, const std::size_t sizeInBytes)
{
    return __asan_region_is_poisoned(start, sizeInBytes) != nullptr;
}

#else // !MEM_POOL_ASAN

inline void poisonMemRegion(void const * /*addr*/, std::size_t /*sizeInBytes*/) noexcept
{
    // No-op
}
inline void unpoisonMemRegion(void const * /*addr*/, std::size_t /*sizeInBytes*/) noexcept
{
    // No-op
}
inline bool addressIsPoisoned(void const * /*addr*/) noexcept
{
    return false;
}
inline bool regionIsPoisoned(void * /*start*/, std::size_t /*size*/) noexcept
{
    return false;
}

#endif // MEM_POOL_ASAN

} // namespace ASan

// ========================================================
// Compile-time maximum of an arbitrary number of values:
// ========================================================

template<typename T>
constexpr T maxOf2(T a, T b) { return (a > b) ? a : b; }

template<typename T>
constexpr T maxOfN(T x) { return x; }

template<typename T, typename... Args>
constexpr T maxOfN(T x, Args... args) { return maxOf2(x, maxOfN(args...)); }

} // namespace MemUtils

// ================================================================================================
//
//                            Dynamic fixed-size-chunk growable pool
//
// ================================================================================================

#ifndef DYNAMIC_POOL_DEFAULT_GRANULARITY
#define DYNAMIC_POOL_DEFAULT_GRANULARITY 64
#endif // DYNAMIC_POOL_DEFAULT_GRANULARITY

// ========================================================
// template class DynamicPool<T, Granularity>:
// ========================================================

//
// Pool of fixed-size memory chunks (similar to a list of arrays).
// Keeps a free-list of deleted memory blocks for recycling.
//
// This pool allocator operates as a linked list of small arrays (chunks).
// Each array is a pool of blocks with the size of 'T' template parameter.
// Template parameter 'Granularity' defines the size in objects of type 'T'
// of such arrays.
//
// allocate() will return an uninitialized memory block.
// The user is responsible for calling construct() on it to run class
// constructors if necessary, and destroy() to call class destructor
// before deallocating the block.
//
// This pool keeps growing for as long as there is memory available in the system.
// Whenever a chunk is exhausted and there are no free blocks to recycle, a new
// chunk is sourced from the heap with MEM_POOL_ALLOC_IMPL (defaults to operator new[]).
//
template
        <
                typename T,
                int Granularity = DYNAMIC_POOL_DEFAULT_GRANULARITY
        >
class DynamicPool final
{
    struct PooledObject;
    struct PoolChunk;

public:

    // Empty pool; no allocation until first use.
    DynamicPool() = default;

    // Drains the pool, freeing every block.
    ~DynamicPool();

    // Not copyable.
    DynamicPool(const DynamicPool &) = delete;
    DynamicPool & operator = (const DynamicPool &) = delete;

    // Allocates a single memory block of size 'T' and returns an uninitialized pointer to it.
    // sizeInBytes can be <= sizeof(T), if you are for instance sharing the same pool for objects of similar size.
    T * allocate(std::size_t sizeInBytes = sizeof(T));

    // Deallocates a memory block previously allocated by 'allocate()'.
    // Pointer may be null, in which case this is a no-op. NOTE: Class destructor NOT called!
    void deallocate(T * ptr, std::size_t sizeInBytes = sizeof(T));

    // Frees all blocks, reseting the pool allocator to its initial state.
    // WARNING: Calling this method will invalidate any memory block still
    // alive that was previously allocated from this pool.
    void drain();

    // Test if the incoming pointer was allocated from this pool and was not freed.
    // NOTE: This is a slow operation. It has to test the pointer against each chunk
    // in the pool; use for debugging only!
    bool isValidPtr(const T * const ptr) const;

    // Miscellaneous runtime debug queries:
#if MEM_POOL_STATS
    int getTotalAllocs() const;
    int getTotalFrees() const;
    int getObjectsAlive() const;
    int getNumberOfChunks() const;
    int getHightWatermark() const;
#endif // MEM_POOL_STATS

    // Static queries:
    static constexpr std::size_t getGranularity();
    static constexpr std::size_t getObjectSize();

    // By default this will only dump the pointers of each allocated object,
    // but you can override this method for each specialized template type,
    // as shown here: https://gist.github.com/glampert/127eb65a71020206d970f0ec64394666
#if MEM_POOL_DEBUG
    static void poolDumpCallback(const DynamicPool & pool);
#endif // MEM_POOL_DEBUG

    //
    // Pool iterator:
    //
    template<typename PooledType, typename ChunkType>
    class FwdIterator final
            : public std::iterator<std::forward_iterator_tag, PooledType>
    {
    public:
        using value     = PooledType;
        using pointer   = PooledType *;
        using reference = PooledType &;

        // Construct an invalid iterator (== end())
        FwdIterator() = default;

        // Construct at any index in the pool
        FwdIterator(ChunkType * chunk, const std::size_t objIndex = 0)
                : m_currChunk{ chunk }
                , m_currObjIndex{ objIndex }
        { }

        bool valid() const
        {
            return (m_currChunk != nullptr && m_currObjIndex < Granularity);
        }

        reference operator*() const
        {
                    MEM_POOL_ASSERT(valid());
            return *reinterpret_cast<pointer>(&(m_currChunk->objects[m_currObjIndex]));
        }

        reference operator->() const
        {
                    MEM_POOL_ASSERT(valid());
            return *reinterpret_cast<pointer>(&(m_currChunk->objects[m_currObjIndex]));
        }

        FwdIterator operator++() // Pre-increment (++i)
        {
                    MEM_POOL_ASSERT(valid());
            do {
                if ((++m_currObjIndex) == Granularity)
                {
                    m_currChunk = m_currChunk->next;
                    m_currObjIndex = ((m_currChunk != nullptr) ? 0 : Granularity);
                }
            } while (valid() && !m_currChunk->objects[m_currObjIndex].isAlive());
            return *this;
        }

        FwdIterator operator++(int) // Post-increment (i++)
        {
            auto temp{ *this };
            ++(*this);
            return temp;
        }

        bool operator == (const FwdIterator & other) const
        {
            return (m_currChunk == other.m_currChunk && m_currObjIndex == other.m_currObjIndex);
        }

        bool operator != (const FwdIterator & other) const
        {
            return !(*this == other);
        }

        // One way conversion: iterator -> const_iterator
        operator FwdIterator<const PooledType, const ChunkType>() const
        {
            return { m_currChunk, m_currObjIndex };
        }

    private:

        ChunkType * m_currChunk    = nullptr;
        std::size_t m_currObjIndex = 0;
    };

    using iterator       = FwdIterator<T, PoolChunk>;
    using const_iterator = FwdIterator<const T, const PoolChunk>;

    iterator       begin()        { return { m_chunksList }; }
    const_iterator begin()  const { return { m_chunksList }; }
    const_iterator cbegin() const { return { m_chunksList }; }

    iterator       end()          { return { nullptr, (m_chunksList ? Granularity : 0u) }; }
    const_iterator end()    const { return { nullptr, (m_chunksList ? Granularity : 0u) }; }
    const_iterator cend()   const { return { nullptr, (m_chunksList ? Granularity : 0u) }; }

    //
    // For-each functional iteration style.
    // The callback returns true to continue iterating or false to stop it.
    //
    template<typename F>
    void forEach(F && func)
    {
        for (auto iter = begin(); iter != end(); ++iter)
        {
            if (!func(*iter)) { break; }
        }
    }
    template<typename F>
    void forEach(F && func) const
    {
        for (auto iter = cbegin(); iter != cend(); ++iter)
        {
            if (!func(*iter)) { break; }
        }
    }

private:

    struct PooledObject
    {
        union Payload
        {
            // Link in the free list (if recycled).
            PooledObject * next;

            // Memory block for the user object, with correct alignment.
            alignas(T) std::uint8_t block[sizeof(T)];
        } u;

        // Guard footer for AddressSanitizer and also the pool flags
        // (only the dead/alive flag (idx:0) used for now).
#if MEM_POOL_ASAN
        std::uint8_t padding[8];
        std::uint8_t flags[8];
#else // !MEM_POOL_ASAN
        std::uint8_t flags[1];
#endif // MEM_POOL_ASAN

        bool isAlive() const { return flags[0] != 0; }
        void setAlive(const std::uint8_t f) { flags[0] = f; }
    };

    struct PoolChunk
    {
        // User items (or recycled links in the free list).
        PooledObject objects[Granularity];

        // This region is never used, but we poison it when using AddressSanitizer,
        // so if the last object in the above pool chunk attempts to read or write
        // beyond its size, we will get a clean crash, rather than corrupting the
        // next pointer, which is not poisoned to make iterating the pool possible.
#if MEM_POOL_ASAN
        std::uint8_t padding[8];
#endif // MEM_POOL_ASAN

        // Link in the list of chunks
        PoolChunk * next;
    };

    // Pool lists:
    PoolChunk    * m_chunksList = nullptr; // List of all chunks ever allocated.
    PooledObject * m_freeList   = nullptr; // List of free objects that can be recycled.

    // Optional debugging stats:
#if MEM_POOL_STATS
    std::int32_t m_allocCount    = 0; // Total calls to 'allocate()'.
    std::int32_t m_objectCount   = 0; // User objects ('T' instances) currently alive.
    std::int32_t m_chunkCount    = 0; // Size/length of 'm_chunksList'.
    std::int32_t m_highWatermark = 0; // Largest number of user objects allocated so far.
#endif // MEM_POOL_STATS
};

// ========================================================
// DynamicPool inline implementation:
// ========================================================

template<typename T, int Granularity>
DynamicPool<T, Granularity>::~DynamicPool()
{
    drain();
}

template<typename T, int Granularity>
T * DynamicPool<T, Granularity>::allocate(std::size_t sizeInBytes)
{
            MEM_POOL_ASSERT(sizeInBytes <= sizeof(T));

    // We need to unpoison at least a whole pointer to access the free list.
    if (sizeInBytes < sizeof(void *))
    {
        sizeInBytes = sizeof(void *);
    }

    // No objects to recycle? Allocate a new chunk.
    if (m_freeList == nullptr)
    {
        PoolChunk * newChunk = MEM_POOL_ALLOC_IMPL(PoolChunk, 1);
        MemUtils::debugMemFill(newChunk, sizeof(*newChunk), MemUtils::AllocFillVal);

        newChunk->next = m_chunksList;
        m_chunksList   = newChunk;

        MEM_POOL_STATS_ONLY(++m_chunkCount);

        // All objects in the new pool chunk are appended
        // to the free list, since they are ready to be used.
        for (int i = 0; i < Granularity; ++i)
        {
            newChunk->objects[i].setAlive(false);
            newChunk->objects[i].u.next = m_freeList;
            m_freeList = &newChunk->objects[i];
        }

        // If running AddressSanitizer, make the chunk padding inaccessible.
#if MEM_POOL_ASAN
        MemUtils::ASan::poisonMemRegion(newChunk->padding, sizeof(newChunk->padding));
#endif // MEM_POOL_ASAN
    }

#if MEM_POOL_STATS
    ++m_allocCount;
    ++m_objectCount;
    if (m_objectCount > m_highWatermark)
    {
        m_highWatermark = m_objectCount;
    }
#endif // MEM_POOL_STATS

    // Poison the whole block to cover the debug padding, then selectively
    // allow access to just the user portion and the internal flags.
    MemUtils::ASan::poisonMemRegion(m_freeList, sizeof(*m_freeList));
    MemUtils::ASan::unpoisonMemRegion(&m_freeList->u, sizeInBytes);
    MemUtils::ASan::unpoisonMemRegion(&m_freeList->flags, sizeof(m_freeList->flags));

    // Pop one from the free list's head:
    PooledObject * object = m_freeList;
    m_freeList = m_freeList->u.next;
    object->setAlive(true);

    // Debug fill the user object with a known pattern
    // to help detecting the use of uninitialized variables.
    MemUtils::debugMemFill(object, sizeInBytes, MemUtils::AllocFillVal);

    return reinterpret_cast<T *>(object);
}

template<typename T, int Granularity>
void DynamicPool<T, Granularity>::deallocate(T * ptr, std::size_t sizeInBytes)
{
            MEM_POOL_ASSERT(m_chunksList != nullptr); // Can't deallocate from an empty pool

    if (ptr == nullptr)
    {
        return;
    }

            MEM_POOL_ASSERT(isValidPtr(ptr));
    auto object = reinterpret_cast<PooledObject *>(ptr);

    // Minimal allocation size.
    if (sizeInBytes < sizeof(void *))
    {
        sizeInBytes = sizeof(void *);
    }

    // Fill user portion with a known pattern to help
    // detecting post-deallocation usage attempts.
    MemUtils::debugMemFill(object, sizeInBytes, MemUtils::FreeFillVal);

    // Add back to free list's head. Memory not actually freed but recycled at a future allocation.
    object->u.next = m_freeList;
    m_freeList = object;
    object->setAlive(false);
    MEM_POOL_STATS_ONLY(--m_objectCount);

    // Make it inaccessible, so we catch any user-after-free attempts with AddressSanitizer.
    MemUtils::ASan::poisonMemRegion(object, sizeof(*object) - sizeof(object->flags)); // minus the pool flags
}

template<typename T, int Granularity>
void DynamicPool<T, Granularity>::drain()
{
    while (m_chunksList != nullptr)
    {
        // Make sure the whole chunk is accessible, otherwise we wouldn't be able to delete it!
        MemUtils::ASan::unpoisonMemRegion(m_chunksList, sizeof(*m_chunksList));

        PoolChunk * chunk = m_chunksList;
        m_chunksList = m_chunksList->next;

        MEM_POOL_FREE_IMPL(chunk, 1);
    }

    m_chunksList = nullptr;
    m_freeList   = nullptr;

#if MEM_POOL_STATS
    m_allocCount    = 0;
    m_objectCount   = 0;
    m_chunkCount    = 0;
    m_highWatermark = 0;
#endif // MEM_POOL_STATS
}

template<typename T, int Granularity>
bool DynamicPool<T, Granularity>::isValidPtr(const T * const ptr) const
{
    if (ptr == nullptr || m_chunksList == nullptr)
    {
        return false;
    }

    // Linear in the number of allocated chunks!
    for (const PoolChunk * chunk = m_chunksList; chunk != nullptr; chunk = chunk->next)
    {
        const auto userAddr  = reinterpret_cast<const std::uint8_t *>(ptr);
        const auto startAddr = reinterpret_cast<const std::uint8_t *>(chunk->objects);
        const auto endAddr   = startAddr + (sizeof(PooledObject) * Granularity);

        if (userAddr >= startAddr && userAddr < endAddr)
        {
            return reinterpret_cast<const PooledObject *>(userAddr)->isAlive();
        }
    }

    return false;
}

#if MEM_POOL_STATS

template<typename T, int Granularity>
int DynamicPool<T, Granularity>::getTotalAllocs() const
{
    return m_allocCount;
}

template<typename T, int Granularity>
int DynamicPool<T, Granularity>::getTotalFrees() const
{
    return m_allocCount - m_objectCount;
}

template<typename T, int Granularity>
int DynamicPool<T, Granularity>::getObjectsAlive() const
{
    return m_objectCount;
}

template<typename T, int Granularity>
int DynamicPool<T, Granularity>::getNumberOfChunks() const
{
    return m_chunkCount;
}

template<typename T, int Granularity>
int DynamicPool<T, Granularity>::getHightWatermark() const
{
    return m_highWatermark;
}

#endif // MEM_POOL_STATS

template<typename T, int Granularity>
constexpr std::size_t DynamicPool<T, Granularity>::getGranularity()
{
    return Granularity;
}

template<typename T, int Granularity>
constexpr std::size_t DynamicPool<T, Granularity>::getObjectSize()
{
    return sizeof(T);
}

#if MEM_POOL_DEBUG

template<typename T, int Granularity>
void DynamicPool<T, Granularity>::poolDumpCallback(const DynamicPool & pool)
{
    std::fprintf(stderr, "======= DynamicPool<%s> Dump Callback =======\n", typeid(T).name());
    std::fprintf(stderr, "Object size.....: %zu\n", pool.getObjectSize());
    std::fprintf(stderr, "Granularity.....: %zu\n", pool.getGranularity());

    #if MEM_POOL_STATS
    std::fprintf(stderr, "Alloc count.....: %i\n",  pool.getTotalAllocs());
    std::fprintf(stderr, "Free count......: %i\n",  pool.getTotalFrees());
    std::fprintf(stderr, "Alive count.....: %i\n",  pool.getObjectsAlive());
    std::fprintf(stderr, "Pool chunks.....: %i\n",  pool.getNumberOfChunks());
    std::fprintf(stderr, "High watermark..: %i\n",  pool.getHightWatermark());
    #endif // MEM_POOL_STATS

    std::fprintf(stderr, "-- Dumping all alive pointers:\n");

    int i = 0;
    for (auto iter = pool.cbegin(); iter != pool.cend(); ++iter, ++i)
    {
        const auto ptr64 = static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&(*iter)));
        std::fprintf(stderr, "[%02i]: 0x%llX\n", i, ptr64);
    }

    std::fprintf(stderr, "=============================================\n");
}

#endif // MEM_POOL_DEBUG

// ================================================================================================
//
//                                      Fixed-size pools
//
// ================================================================================================

#ifndef FIXED_SIZE_POOL_DEFAULT_SIZE
#define FIXED_SIZE_POOL_DEFAULT_SIZE 128
#endif // FIXED_SIZE_POOL_DEFAULT_SIZE

// ========================================================
// template class FixedSizePoolBase<T>:
// ========================================================

//
// Base with common operations for the fixed-size pools.
// Unlike DynamicPool, this pool has a set number of slots
// defined upon construction. Once all slots are allocated
// an overflow error is generated and the pool return null.
//
template<typename T>
class FixedSizePoolBase
{
protected:

    // Protected and non-virtual.
    ~FixedSizePoolBase() = default;

    struct PooledObject
    {
        union Payload
        {
            // Index in the free list (if recycled).
            std::int32_t next;

            // Memory block for the user object, with correct alignment.
            alignas(T) std::uint8_t block[sizeof(T)];
        } u;

        // Guard footer for AddressSanitizer to detect intra-object overflows.
#if MEM_POOL_ASAN
        std::uint8_t padding[8];
#endif // MEM_POOL_ASAN
    };

    using PoolFlag = std::uint8_t;
    static constexpr std::int32_t InvalidSlotIndex = -1;

    int getIndex(const void * const ptr) const;
    void buildFreeList();

    PooledObject * getSlots();
    const PooledObject * getSlots() const;

    PoolFlag * getFlags();
    const PoolFlag * getFlags() const;

public:

    FixedSizePoolBase(PooledObject * slots, std::uint8_t * flags, int slotCount);

    // Not copyable.
    FixedSizePoolBase(const FixedSizePoolBase &) = delete;
    FixedSizePoolBase & operator = (const FixedSizePoolBase &) = delete;

    // Allocates a single memory block of size 'T' and returns an uninitialized pointer to it.
    // sizeInBytes can be <= sizeof(T), if you are for instance sharing the same pool for objects of similar size.
    T * allocate(std::size_t sizeInBytes = sizeof(T));

    // Deallocates a memory block previously allocated by 'allocate()'.
    // Pointer may be null, in which case this is a no-op. NOTE: Class destructor NOT called!
    void deallocate(T * ptr, std::size_t sizeInBytes = sizeof(T));

    // Frees all blocks, reseting the pool allocator to its initial state.
    // WARNING: Calling this method will invalidate any memory block still
    // alive that was previously allocated from this pool.
    void drain();

    // Test if the incoming pointer was allocated from this pool and was not freed.
    // NOTE: Unlike for the DynamicPool, this method is cheap constant time.
    bool isValidPtr(const T * const ptr) const;

    // Miscellaneous pool stats:
    bool isFull() const;
    bool isEmpty() const;
    int getSlotCount() const;
    int getSlotsUsed() const;
    int getFirstFreeIndex() const;
    static constexpr std::size_t getObjectSize();

    // Debug-only stats:
#if MEM_POOL_STATS
    int getTotalAllocs() const;
    int getTotalFrees() const;
    int getObjectsAlive() const;
    int getHightWatermark() const;
#endif // MEM_POOL_STATS

    // Function automatically called if the pool overflows. Can also be called
    // at any time to dump the contents of the pool (alive objects only).
    // By default this will only dump the pointers of each allocated object,
    // but you can override this method for each specialized template type,
    // as shown here: https://gist.github.com/glampert/127eb65a71020206d970f0ec64394666
#if MEM_POOL_DEBUG
    static void poolDumpCallback(const FixedSizePoolBase & pool);
#endif // MEM_POOL_DEBUG

    //
    // Pool iterator:
    //
    template<typename PooledType, typename PoolSlotType>
    class FwdIterator final
            : public std::iterator<std::forward_iterator_tag, PooledType>
    {
    public:
        using value     = PooledType;
        using pointer   = PooledType *;
        using reference = PooledType &;

        // Construct an invalid iterator (== end())
        FwdIterator() = default;

        // Construct at any index in the pool
        FwdIterator(PoolSlotType * const slots, const PoolFlag * const flags,
                    const std::int32_t slotCount, const std::int32_t objIndex = 0)
                : m_poolSlots{ slots }
                , m_poolFlags{ flags }
                , m_poolSize{ slotCount }
                , m_currObjIndex{ objIndex }
        { }

        bool valid() const
        {
            return (m_currObjIndex < m_poolSize &&
                    m_poolSlots != nullptr &&
                    m_poolFlags != nullptr);
        }

        reference operator*() const
        {
                    MEM_POOL_ASSERT(valid());
            return *reinterpret_cast<pointer>(&m_poolSlots[m_currObjIndex]);
        }

        reference operator->() const
        {
                    MEM_POOL_ASSERT(valid());
            return *reinterpret_cast<pointer>(&m_poolSlots[m_currObjIndex]);
        }

        FwdIterator operator++() // Pre-increment (++i)
        {
                    MEM_POOL_ASSERT(valid());
            // Skip over deallocated objects.
            do {
                if ((++m_currObjIndex) == m_poolSize)
                {
                    break;
                }
            } while (m_poolFlags[m_currObjIndex] == 0);
            return *this;
        }

        FwdIterator operator++(int) // Post-increment (i++)
        {
            auto temp{ *this };
            ++(*this);
            return temp;
        }

        bool operator == (const FwdIterator & other) const
        {
            return (m_poolSlots == other.m_poolSlots && m_currObjIndex == other.m_currObjIndex);
        }

        bool operator != (const FwdIterator & other) const
        {
            return !(*this == other);
        }

        // One way conversion: iterator -> const_iterator
        operator FwdIterator<const PooledType, const PoolSlotType>() const
        {
            return { m_poolSlots, m_poolFlags, m_poolSize, m_currObjIndex };
        }

    private:

        PoolSlotType   * m_poolSlots    = nullptr;
        const PoolFlag * m_poolFlags    = nullptr;
        std::int32_t     m_poolSize     = 0;
        std::int32_t     m_currObjIndex = 0;
    };

    using iterator       = FwdIterator<T, PooledObject>;
    using const_iterator = FwdIterator<const T, const PooledObject>;

    iterator       begin()        { return makeIterator(0); }
    const_iterator begin()  const { return makeIterator(0); }
    const_iterator cbegin() const { return makeIterator(0); }

    iterator       end()          { return makeIterator(m_slotCount); }
    const_iterator end()    const { return makeIterator(m_slotCount); }
    const_iterator cend()   const { return makeIterator(m_slotCount); }

    //
    // For-each functional iteration style.
    // The callback returns true to continue iterating or false to stop it.
    //
    template<typename F>
    void forEach(F && func)
    {
        for (auto iter = begin(); iter != end(); ++iter)
        {
            if (!func(*iter)) { break; }
        }
    }
    template<typename F>
    void forEach(F && func) const
    {
        for (auto iter = cbegin(); iter != cend(); ++iter)
        {
            if (!func(*iter)) { break; }
        }
    }

private:

    iterator makeIterator(const std::int32_t objIndex)
    {
        return (!isEmpty() ? iterator{ m_slots, m_flags, m_slotCount, objIndex } : iterator{});
    }
    const_iterator makeIterator(const std::int32_t objIndex) const
    {
        return (!isEmpty() ? const_iterator{ m_slots, m_flags, m_slotCount, objIndex } : const_iterator{});
    }

    // Pool guts:
    PooledObject * m_slots;
    PoolFlag     * m_flags;
    std::int32_t   m_slotCount;
    std::int32_t   m_slotsUsed;
    std::int32_t   m_firstFreeIndex;

    // Optional debugging stats:
#if MEM_POOL_STATS
    std::int32_t m_allocCount    = 0; // Total calls to 'allocate()'.
    std::int32_t m_objectCount   = 0; // User objects ('T' instances) currently alive.
    std::int32_t m_highWatermark = 0; // Largest number of user objects allocated so far.
#endif // MEM_POOL_STATS
};

// ========================================================
// template class FixedSizePoolInPlace<T, MaxSize>:
// ========================================================

//
// Fixed-size pool with in-place storage (inline with the object).
//
// The size of the buffer of objects is a template parameter.
// No additional dynamic memory is allocated by this type of pool.
//
template
        <
                typename T,
                int MaxSize = FIXED_SIZE_POOL_DEFAULT_SIZE
        >
class FixedSizePoolInPlace final
        : public FixedSizePoolBase<T>
{
public:

    using Base = FixedSizePoolBase<T>;

    FixedSizePoolInPlace()
            : Base{ m_objStore, m_objFlags, MaxSize }
    { }

    ~FixedSizePoolInPlace()
    {
        // Make the whole backing-store accessible again, in case this object was declared on the stack.
        MemUtils::ASan::unpoisonMemRegion(m_objStore, sizeof(m_objStore));

        // Steamroll the memory once the pool is destroyed.
        // This might catch attempts to use pointers from the deleted pool.
        MemUtils::debugMemFill(m_objStore, sizeof(m_objStore), MemUtils::FreeFillVal);
    }

#if MEM_POOL_DEBUG
    static void poolDumpCallback(const Base & pool) { Base::poolDumpCallback(pool); }
#endif // MEM_POOL_DEBUG

private:

    using PooledObject = typename Base::PooledObject;
    using PoolFlag     = typename Base::PoolFlag;

    PooledObject m_objStore[MaxSize];
    PoolFlag     m_objFlags[MaxSize];
};

// ========================================================
// template class FixedSizePoolDynamic<T, DefaultSize>:
// ========================================================

//
// Fixed-size pool that dynamically allocates its buffer
// of objects upon construction. The pool never grows.
//
// A default size template parameter can be provided, however,
// the size given to the constructor is the one used to allocate
// the underlaying object store.
//
template
        <
                typename T,
                int DefaultSize = FIXED_SIZE_POOL_DEFAULT_SIZE
        >
class FixedSizePoolDynamic final
        : public FixedSizePoolBase<T>
{
public:

    using Base = FixedSizePoolBase<T>;

    FixedSizePoolDynamic(const int slotCount = DefaultSize)
            : Base{ MEM_POOL_ALLOC_IMPL(PooledObject, slotCount),
                    MEM_POOL_ALLOC_IMPL(PoolFlag,     slotCount),
                    slotCount }
    { }

    ~FixedSizePoolDynamic()
    {
        // Make sure we can free the whole memory block.
        MemUtils::ASan::unpoisonMemRegion(Base::getSlots(), Base::getSlotCount() * sizeof(PooledObject));

        MEM_POOL_FREE_IMPL(Base::getSlots(), Base::getSlotCount());
        MEM_POOL_FREE_IMPL(Base::getFlags(), Base::getSlotCount());
    }

#if MEM_POOL_DEBUG
    static void poolDumpCallback(const Base & pool) { Base::poolDumpCallback(pool); }
#endif // MEM_POOL_DEBUG

private:

    using PooledObject = typename Base::PooledObject;
    using PoolFlag     = typename Base::PoolFlag;
};

// ========================================================
// FixedSizePoolBase inline implementation:
// ========================================================

template<typename T>
FixedSizePoolBase<T>::FixedSizePoolBase(PooledObject * slots, std::uint8_t * flags, const int slotCount)
        : m_slots{ slots }
        , m_flags{ flags }
        , m_slotCount{ slotCount }
        , m_slotsUsed{ 0 }
        , m_firstFreeIndex{ InvalidSlotIndex }
{
            MEM_POOL_ASSERT(slots != nullptr);
            MEM_POOL_ASSERT(flags != nullptr);
            MEM_POOL_ASSERT(slotCount != 0);

    buildFreeList();
}

template<typename T>
T * FixedSizePoolBase<T>::allocate(std::size_t sizeInBytes)
{
            MEM_POOL_ASSERT(sizeInBytes <= sizeof(T));

    // No more space?
    if (m_firstFreeIndex == InvalidSlotIndex)
    {
#if MEM_POOL_DEBUG
        poolDumpCallback(*this);
#endif // MEM_POOL_DEBUG

                MEM_POOL_ASSERT(isFull()); // Unless the free list is corrupted, should be true.
        return nullptr;
    }

    // Min size we need to unpoison to access the free list.
    if (sizeInBytes < sizeof(std::int32_t))
    {
        sizeInBytes = sizeof(std::int32_t);
    }

    // Pop one for the free list:
    const auto objIndex = m_firstFreeIndex;
    PooledObject * object = &m_slots[objIndex];
    m_firstFreeIndex = object->u.next;
    m_flags[objIndex] = 1; // Mark in use

    // Unpoison the exact requested size, so we keep the inter-object padding inaccessible.
    MemUtils::ASan::unpoisonMemRegion(object, sizeInBytes);

    // Debug fill the user object with a known pattern
    // to help detecting the use of uninitialized variables.
    MemUtils::debugMemFill(object, sizeInBytes, MemUtils::AllocFillVal);

#if MEM_POOL_STATS
    ++m_allocCount;
    ++m_objectCount;
    if (m_objectCount > m_highWatermark)
    {
        m_highWatermark = m_objectCount;
    }
#endif // MEM_POOL_STATS

    ++m_slotsUsed;
    return reinterpret_cast<T *>(object);
}

template<typename T>
void FixedSizePoolBase<T>::deallocate(T * ptr, std::size_t sizeInBytes)
{
            MEM_POOL_ASSERT(!isEmpty()); // Can't deallocate from an empty pool

    if (ptr == nullptr)
    {
        return;
    }

            MEM_POOL_ASSERT(isValidPtr(ptr));
    auto object = reinterpret_cast<PooledObject *>(ptr);

    // Minimal allocation size.
    if (sizeInBytes < sizeof(std::int32_t))
    {
        sizeInBytes = sizeof(std::int32_t);
    }

    // Fill user portion with a known pattern to help
    // detecting post-deallocation usage attempts.
    MemUtils::debugMemFill(object, sizeInBytes, MemUtils::FreeFillVal);

    // Add back to free list's head. Memory not actually freed but recycled at a future allocation.
    const int objIndex = getIndex(object);
    object->u.next     = m_firstFreeIndex;
    m_firstFreeIndex   = objIndex;
    m_flags[objIndex]  = 0;

    MEM_POOL_STATS_ONLY(--m_objectCount);
    --m_slotsUsed;

    // Poison the whole object minus the first 32 bytes of the free list next index.
    MemUtils::ASan::poisonMemRegion(reinterpret_cast<std::uint8_t *>(object) + sizeof(std::int32_t),
                                    sizeof(PooledObject) - sizeof(std::int32_t));
}

template<typename T>
void FixedSizePoolBase<T>::drain()
{
    m_slotsUsed = 0;
    m_firstFreeIndex = InvalidSlotIndex;

    buildFreeList();

#if MEM_POOL_STATS
    m_allocCount    = 0;
    m_objectCount   = 0;
    m_highWatermark = 0;
#endif // MEM_POOL_STATS
}

template<typename T>
bool FixedSizePoolBase<T>::isValidPtr(const T * const ptr) const
{
    if (ptr == nullptr || m_slotsUsed == 0)
    {
        return false;
    }

    const auto userAddr  = reinterpret_cast<const std::uint8_t *>(ptr);
    const auto startAddr = reinterpret_cast<const std::uint8_t *>(m_slots);
    const auto endAddr   = startAddr + (sizeof(PooledObject) * m_slotCount);

    if (userAddr >= startAddr && userAddr < endAddr)
    {
        const int objIndex = getIndex(ptr);
        return m_flags[objIndex] != 0; // Is the object alive?
    }

    return false;
}

template<typename T>
int FixedSizePoolBase<T>::getIndex(const void * const ptr) const
{
    const auto userAddr  = static_cast<const std::uint8_t *>(ptr);

    const auto startAddr = reinterpret_cast<const std::uint8_t *>(m_slots);
    const auto endAddr   = startAddr + (sizeof(PooledObject) * m_slotCount);

    const auto ptrDiff   = (reinterpret_cast<const PooledObject *>(endAddr) -
                            reinterpret_cast<const PooledObject *>(userAddr));

    const int objIndex = static_cast<int>(m_slotCount - ptrDiff);
            MEM_POOL_ASSERT(objIndex >= 0 && objIndex < m_slotCount);
    return objIndex;
}

template<typename T>
void FixedSizePoolBase<T>::buildFreeList()
{
    MemUtils::debugMemFill(m_slots, m_slotCount * sizeof(*m_slots), MemUtils::AllocFillVal);
    std::memset(m_flags, 0, m_slotCount * sizeof(*m_flags));

    const int count = m_slotCount;
    PooledObject * const slots = m_slots;

    for (int i = 0; i < count; ++i)
    {
        slots[i].u.next  = m_firstFreeIndex;
        m_firstFreeIndex = i;

        // Poison the whole object minus the first 32 bytes of the free list next index.
        MemUtils::ASan::poisonMemRegion(reinterpret_cast<std::uint8_t *>(&slots[i]) + sizeof(std::int32_t),
                                        sizeof(PooledObject) - sizeof(std::int32_t));
    }
}

template<typename T>
typename FixedSizePoolBase<T>::PooledObject * FixedSizePoolBase<T>::getSlots()
{
    return m_slots;
}

template<typename T>
const typename FixedSizePoolBase<T>::PooledObject * FixedSizePoolBase<T>::getSlots() const
{
    return m_slots;
}

template<typename T>
typename FixedSizePoolBase<T>::PoolFlag * FixedSizePoolBase<T>::getFlags()
{
    return m_flags;
}

template<typename T>
const typename FixedSizePoolBase<T>::PoolFlag * FixedSizePoolBase<T>::getFlags() const
{
    return m_flags;
}

template<typename T>
bool FixedSizePoolBase<T>::isFull() const
{
    return (m_slotsUsed == m_slotCount);
}

template<typename T>
bool FixedSizePoolBase<T>::isEmpty() const
{
    return (m_slotsUsed == 0);
}

template<typename T>
int FixedSizePoolBase<T>::getSlotCount() const
{
    return m_slotCount;
}

template<typename T>
int FixedSizePoolBase<T>::getSlotsUsed() const
{
    return m_slotsUsed;
}

template<typename T>
int FixedSizePoolBase<T>::getFirstFreeIndex() const
{
    return m_firstFreeIndex;
}

template<typename T>
constexpr std::size_t FixedSizePoolBase<T>::getObjectSize()
{
    return sizeof(T);
}

#if MEM_POOL_STATS

template<typename T>
int FixedSizePoolBase<T>::getTotalAllocs() const
{
    return m_allocCount;
}

template<typename T>
int FixedSizePoolBase<T>::getTotalFrees() const
{
    return m_allocCount - m_objectCount;
}

template<typename T>
int FixedSizePoolBase<T>::getObjectsAlive() const
{
    return m_objectCount;
}

template<typename T>
int FixedSizePoolBase<T>::getHightWatermark() const
{
    return m_highWatermark;
}

#endif // MEM_POOL_STATS

#if MEM_POOL_DEBUG

template<typename T>
void FixedSizePoolBase<T>::poolDumpCallback(const FixedSizePoolBase & pool)
{
    std::fprintf(stderr, "====== FixedSizePool<%s> Dump Callback ======\n", typeid(T).name());
    std::fprintf(stderr, "Object size.....: %zu\n", pool.getObjectSize());
    std::fprintf(stderr, "Slot count......: %i\n",  pool.getSlotCount());
    std::fprintf(stderr, "Slots used......: %i\n",  pool.getSlotsUsed());
    std::fprintf(stderr, "First free idx..: %i\n",  pool.getFirstFreeIndex());

    #if MEM_POOL_STATS
    std::fprintf(stderr, "Alloc count.....: %i\n",  pool.getTotalAllocs());
    std::fprintf(stderr, "Free count......: %i\n",  pool.getTotalFrees());
    std::fprintf(stderr, "Alive count.....: %i\n",  pool.getObjectsAlive());
    std::fprintf(stderr, "High watermark..: %i\n",  pool.getHightWatermark());
    #endif // MEM_POOL_STATS

    std::fprintf(stderr, "-- Dumping all alive pointers:\n");

    const int count = pool.getSlotCount();
    const PoolFlag * const flags = pool.getFlags();
    const PooledObject * const slots = pool.getSlots();

    for (int i = 0; i < count; ++i)
    {
        if (flags[i] != 0)
        {
            const auto ptr64 = static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&slots[i]));
            std::fprintf(stderr, "[%02i]: 0x%llX\n", i, ptr64);
        }
    }

    std::fprintf(stderr, "=============================================\n");
}

#endif // MEM_POOL_DEBUG

// ================================================================================================
//
//                      AddressSanitizer tests for the memory pools
//
// ================================================================================================

#if MEM_POOL_TEST_UNIT_TESTS

// clang++ -std=c++11 -fno-exceptions -Wall -Wextra -pedantic -O1 -g -fno-omit-frame-pointer -fsanitize=address mem_pools.cpp

#include <type_traits>
#include <algorithm>
#include <vector>
#include <cstdio>

#ifdef _MSC_VER
    #define MEM_POOL_TEST_FUNC_NAME __FUNCTION__
#else // !_MSC_VER
    #define MEM_POOL_TEST_FUNC_NAME __func__
#endif // _MSC_VER

#if (!defined(MEM_POOL_TESTS_ASAN_CRASHES) && MEM_POOL_ASAN)
    #define MEM_POOL_TESTS_ASAN_CRASHES 1
#endif // MEM_POOL_TESTS_ASAN_CRASHES

template<template<typename T, int S> class PoolType>
struct MemPoolUnitTests final
{
    // ========================================================
    // Test case: class hierarchies sharing the same pool
    // ========================================================

    struct BaseClass
    {
        std::int32_t i32 = 32;
        void printBase() { std::printf("BaseClass - OK; sizeof=%zu, i32=%i\n", sizeof(*this), i32); }
        virtual ~BaseClass() = default;
    };

    struct Derived1 : BaseClass
    {
        std::uint32_t u32 = 32;
        void printDerived1() { std::printf("Derived1  - OK; sizeof=%zu, u32=%u\n", sizeof(*this), u32); }
        virtual ~Derived1() = default;
    };

    struct Derived2 : Derived1
    {
        std::int64_t i64 = 64;
        void printDerived2() { std::printf("Derived2  - OK; sizeof=%zu, i64=%lli\n", sizeof(*this), i64); }
        virtual ~Derived2() = default;
    };

    struct Derived3 : Derived2
    {
        std::uint64_t u64 = 64;
        void printDerived3() { std::printf("Derived3  - OK; sizeof=%zu, u64=%llu\n", sizeof(*this), u64); }
        virtual ~Derived3() = default;
    };

    static void testClassHierarchiesSharingSamePool()
    {
        std::printf(">> %s()\n", MEM_POOL_TEST_FUNC_NAME);

        constexpr std::size_t MaxPoolSize  = MemUtils::maxOfN(sizeof(BaseClass),  sizeof(Derived1),  sizeof(Derived2),  sizeof(Derived3));
        constexpr std::size_t MaxPoolAlign = MemUtils::maxOfN(alignof(BaseClass), alignof(Derived1), alignof(Derived2), alignof(Derived3));
        using PoolStorage = typename std::aligned_storage<MaxPoolSize, MaxPoolAlign>::type;

        std::printf("MaxPoolSize  = %zu\n", MaxPoolSize);
        std::printf("MaxPoolAlign = %zu\n", MaxPoolAlign);

        PoolType<PoolStorage, 128> pool;

        BaseClass * b0 = MemUtils::construct(reinterpret_cast<BaseClass *>(pool.allocate(sizeof(BaseClass))));
        Derived1  * d1 = MemUtils::construct(reinterpret_cast<Derived1  *>(pool.allocate(sizeof(Derived1))));
        Derived2  * d2 = MemUtils::construct(reinterpret_cast<Derived2  *>(pool.allocate(sizeof(Derived2))));
        Derived3  * d3 = MemUtils::construct(reinterpret_cast<Derived3  *>(pool.allocate(sizeof(Derived3))));

        b0->printBase();
        d1->printDerived1();
        d2->printDerived2();
        d3->printDerived3();

        // Reading past the end of the class due to a bad type cast: Crashes with ASan
        #if MEM_POOL_TESTS_ASAN_CRASHES
        Derived3 * badCast = static_cast<Derived3 *>(d2);
        badCast->printDerived3();
        #endif // MEM_POOL_TESTS_ASAN_CRASHES
    }

    // ========================================================
    // Test case: use after free
    // ========================================================

    static void testUseAfterFree()
    {
        std::printf(">> %s()\n", MEM_POOL_TEST_FUNC_NAME);

        PoolType<std::size_t, 128> pool;
        std::vector<std::size_t *> ptrs;

        for (std::size_t i = 0; i < 128; ++i)
        {
            std::size_t * a = pool.allocate();
            MEM_POOL_ASSERT(pool.isValidPtr(a) == true);

            ptrs.push_back(a);
            *a = i;
        }

        for (std::size_t i = 0; i < ptrs.size() / 2; ++i)
        {
            pool.deallocate(ptrs[i]);
            MEM_POOL_ASSERT(pool.isValidPtr(ptrs[i]) == false);
        }

        int num = 0;
        std::printf("Pool dump:\n");
        pool.forEach([&num](std::size_t i)
        {
            std::printf(" pool val %i = %zu\n", num++, i);
            return true;
        });
        std::printf("----------\n");
        MEM_POOL_ASSERT(num == 64);

        // Half the pool was already freed.
        #if MEM_POOL_TESTS_ASAN_CRASHES
        for (std::size_t i = 0; i < ptrs.size(); ++i)
        {
            const std::size_t * p = ptrs[i];
            std::printf("%p = %#zx\n", (const void *)p, *p);
        }
        #endif // MEM_POOL_TESTS_ASAN_CRASHES
    }

    // ========================================================
    // Test case: bad type casting
    // ========================================================

    static void testBadTypeCasting()
    {
        std::printf(">> %s()\n", MEM_POOL_TEST_FUNC_NAME);

        PoolType<std::int64_t, 128> pool;
        std::int64_t * p0 = pool.allocate();
        std::int64_t * p1 = pool.allocate();
        std::int64_t * p2 = pool.allocate();
        std::int64_t * p3 = pool.allocate();

        // Legal
        *p0 = 0;
        *p1 = 1;
        *p2 = 2;
        *p3 = 3;

        MEM_POOL_ASSERT(pool.isValidPtr(p0));
        MEM_POOL_ASSERT(pool.isValidPtr(p1));
        MEM_POOL_ASSERT(pool.isValidPtr(p2));
        MEM_POOL_ASSERT(pool.isValidPtr(p3));

        // Only 8 bytes, try writing 10
        #if MEM_POOL_TESTS_ASAN_CRASHES
        constexpr int N = 10;
        #else // !MEM_POOL_TESTS_ASAN_CRASHES
        constexpr int N = 8;
        #endif // MEM_POOL_TESTS_ASAN_CRASHES

        char * s = reinterpret_cast<char *>(p3);
        for (int i = 0; i < N; ++i)
        {
            s[i] = '*';
        }
    }

}; // MemPoolUnitTests

#endif // MEM_POOL_TEST_UNIT_TESTS

#endif // MEMORY_POOLS_HPP