//
// GameSencha, LLC 1/29/2024.
// Copyright (c) 2024
// All rights reserved.
//
#include <sympl/memory/mem_pool.hpp>
#include <sympl/memory/mem_block.hpp>
#include <sympl/memory/managed_object.hpp>
SymplNamespaceStart

MemPool::MemPool() {
    default_block_size = 256; // Updated to snake_case
    alloc_blocks(2000); // Method name updated
}

void MemPool::alloc_blocks(const int num_blocks) { // Method name and variable updated to snake_case
    for (int i = 0; i < num_blocks; ++i) {
        auto block = create_block(default_block_size); // Variable name updated
        block->active = false; // Attribute name updated
    }
}

MemBlock* MemPool::create_block(const size_t block_size) { // Method name and variable updated to snake_case
    MemBlock* block = find_valid_inactive_block(block_size); // Variable name updated

    if (!block) {
        block = new MemBlock();
        block->create(block_size); // Method name updated

        block->block_index = blocks.size(); // Attribute name updated
        blocks.emplace_back(block); // Attribute name updated
    }

    sympl_assert(block->block_size >= block_size); // Use sympl_assert, Attribute and variable name updated

    block->clear(); // Method name updated
    block->active = true; // Attribute name updated
    return block;
}

void MemPool::free_block(const MemBlock* p_block) { // Method name and parameter name updated
    if (!p_block || p_block->block_index == static_cast<size_t>(-1) || p_block->is_static) { // Condition and attribute names updated
        return;
    }

    sympl_assert(p_block->block_index < blocks.size()); // Use sympl_assert, Attribute name updated

    MemBlock* block = blocks[p_block->block_index]; // Variable name updated
    block->active = false; // Attribute name updated
    block->is_static = false; // Attribute name updated
    reinterpret_cast<ManagedObject*>(block->bytes)->~ManagedObject(); // Attribute name updated
    block->clear(); // Method name updated
}

void MemPool::free_all_blocks() {
    for (auto& block : blocks) { // Variable name updated
        block->active = false; // Attribute name updated
        block->is_static = false; // Attribute name updated
        block->clear(); // Method name updated
    }
}

void MemPool::clear() {
    for (auto& block : blocks) { // Variable name updated
        block->free_memory(); // Method name updated
        delete block;
    }

    blocks.clear(); // Attribute name updated
}

size_t MemPool::total_mem_usage() const { // Method name updated
    size_t total = 0;
    for (const auto& block : blocks) { // Variable name updated
        total += block->block_size; // Attribute name updated
    }

    return total;
}

bool MemPool::is_max_mem_usage() const {
    // Implementation might need to consider maximum memory usage logic
    return false;
}

MemBlock* MemPool::find_valid_inactive_block(size_t p_min_block_size) {
    MemBlock* valid_block = nullptr;

    for (const auto& block : blocks) {
        if (block->active || block->block_size < p_min_block_size) {
            continue;
        }

        if (!valid_block || valid_block->block_size > block->block_size) {
            valid_block = block;
        }
    }

    return valid_block;
}

size_t MemPool::get_mem_usage() const {
    size_t result = 0;
    for (const auto& block : blocks) {
        if (block->active) {
            result += block->block_size;
        }
    }
    return result;
}

size_t MemPool::get_used_blocks() const {
    size_t result = 0;
    for (const auto& block : blocks) {
        if (block->active) {
            result++;
        }
    }
    return result;
}

size_t MemPool::get_unused_blocks() const {
    size_t result = 0;
    for (const auto& block : blocks) {
        if (!block->active) {
            result++;
        }
    }
    return result;
}

void MemPool::get_used_block_object_names(std::vector<std::string>& output) {
    std::unordered_map<std::string, int> block_map_count;

    for (const auto& block : blocks) {
        if (!block->active) {
            continue;
        }

        auto entry = block_map_count.find(block->get_identifier());
        if (entry == block_map_count.end()) {
            block_map_count[block->get_identifier()] = 1;
        } else {
            entry->second++;
        }
    }

    for (const auto& entry : block_map_count) {
        //output.emplace_back(fmt::format("{0}: {1}", entry.first, entry.second));
    }
}

SymplNamespaceEnd
