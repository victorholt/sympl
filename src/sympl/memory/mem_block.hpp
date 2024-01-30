//
// GameSencha, LLC 1/29/2024.
// Copyright (c) 2024
// All rights reserved.
//
#pragma once
#include <sympl/sympl_pch.hpp>

SymplNamespaceStart

class SYMPL_API MemBlock
{
protected:
    // Identifier used for debugging.
    char identifier[256];

public:
    // Memory block index.
    size_t block_index = 0;

    // Current size of the block.
    size_t block_size;

    // Max size of the block.
    bool active;

    // Check if this block can be changed.
    bool is_static;

    // Memory of the block.
    StrPtr bytes;

    // Constructor.
    MemBlock();

    // Creates the block with the given size.
    void create(size_t size);

    // Clears out the block memory.
    void clear();

    // Frees the memory.
    void free_memory();

    // Sets the identifier.
    void set_identifier(CStrPtr p_identifier);

    /**
     * Returns the identifier.
     * @return
     */
    inline const char* get_identifier() const { return identifier; }
};

SymplNamespaceEnd
