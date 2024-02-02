//
// GameSencha, LLC 1/29/2024.
// Copyright (c) 2024
// All rights reserved.
//
#pragma once
#include <sympl/sympl_pch.hpp>

SymplNamespaceStart

class MemBlock;

class SYMPL_API MemPool
{
private:
    // Default size of a block.
    size_t default_block_size = 1024;

    // Current blocks in use.
    std::vector<class MemBlock*> blocks;

    /**
     * Constructor.
     */
    MemPool();

    /**
     * Attempts to find a valid inactive block.
     * @param p_min_block_size
     * @return
     */
    class MemBlock* find_valid_inactive_block(size_t p_min_block_size = 0);

public:
    //! Allocates a certain number of new blocks.
    //! \param num_blocks
    void alloc_blocks(const int num_blocks);

    //! Creates a new block of a given size.
    //! \param block_size
    //! \return 
    MemBlock* create_block(const size_t block_size);

    //! Frees a block from at a given index.
    //! \param p_block
    void free_block(const class MemBlock* p_block);

    //! Frees all blocks.
    void free_all_blocks();

    //! Free all blocks in the pool.
    void clear();

    //! Retrieves the total memory usage.
    //! \return
    size_t total_mem_usage() const;

    //! Check if we're at the max mem usage.
    //! \return
    bool is_max_mem_usage() const;

    //! Returns the current memory usage.
    //! \return
    size_t get_mem_usage() const;

    //! Returns the number of used blocks.
    //! \return
    size_t get_used_blocks() const;

    //! Returns the number of unused blocks.
    //! \return
    size_t get_unused_blocks() const;

    //! Returns the object names that are being used.
    //! \param output
    void get_used_block_object_names(std::vector<std::string>& output);

    //! Returns the instance for the pool.
    //! \return
    inline static MemPool* instance()
    {
        static MemPool pool;
        return &pool;
    }
};

SymplNamespaceEnd
