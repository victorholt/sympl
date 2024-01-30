//
// GameSencha, LLC 1/29/2024.
// Copyright (c) 2024
// All rights reserved.
//
#include "mem_block.hpp"
SymplNamespace

MemBlock::MemBlock() {
    bytes = nullptr;
    block_index = -1; // Consider using size_t and setting to some max value if -1 is used as uninitialized flag
    active = false;
    is_static = false; // Renamed from 'Static' to avoid keyword confusion
}

void MemBlock::create(size_t size) {
    // Free any existing memory before reallocating
    free_memory(); // Changed to use the free_memory method for consistency

    bytes = static_cast<StrPtr>(calloc(1, size));
    if (!bytes) {
        // Handle allocation failure; might set 'active' to false or throw an exception
    }

    block_size = size;
    active = true;
    is_static = false; // Ensuring the block is marked as non-static upon creation

    clear(); // Resets the block's contents
}

void MemBlock::clear() {
    if (!bytes) {
        return;
    }

    memset(bytes, 0, block_size);
    memset(identifier, 0, sizeof(identifier)); // Clear identifier upon clearing the block
}

void MemBlock::free_memory() { // Renamed from 'Free' to 'free_memory' for consistency
    if (!bytes) {
        return;
    }

    free(bytes);
    bytes = nullptr;
    block_size = 0;
    block_index = static_cast<size_t>(-1); // Reset to max size_t value or another designated 'uninitialized' value
    active = false;
    is_static = false;
}

void MemBlock::set_identifier(CStrPtr p_identifier) {
    memset(identifier, 0, sizeof(identifier));
    // Ensure we don't exceed the identifier buffer size
    strncpy(identifier, p_identifier, sizeof(identifier) - 1);
}
