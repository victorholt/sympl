//
// GameSencha, LLC 5/25/22.
//
#pragma once
#include <sympl/SymplPCH.hpp>

SymplNamespaceStart

class MemPool
{
private:
	// Default size of a block.
	size_t DefaultBlockSize = 1024;

	// Current blocks in use.
	std::vector<class MemBlock*> Blocks;

	/**
	 * Constructor.
	 */
	MemPool();

	/**
	 * Attempts to find a valid inactive block.
	 * @param MinBlockSize
	 * @return
	 */
	class MemBlock* FindValidInactiveBlock(size_t MinBlockSize = 0);

public:

    /**
     * Allocates a certain number of new blocks.
     * @param NumBlocks
     */
    void AllocBlock(int NumBlocks);

	/**
	 * Creates a new block of a given size.
	 * @param BlockSize
	 * @return
	 */
	class MemBlock* CreateBlock(size_t BlockSize);

	/**
	 * Frees a block from at a given index.
	 * @param pBlock
	 */
	void FreeBlock(class MemBlock* pBlock);

	/**
	 * Free all blocks in the pool.
	 */
	void Clear();

	/**
	 * Retrieves the total memory usage.
	 * @return
	 */
	size_t TotalMemUsage() const;

	/**
	 * Check if we're at the max mem usage.
	 * @return
	 */
	bool IsMaxMemUsage() const;

	/**
	 * Returns the instance for the pool.
	 * @return
	 */
	inline static MemPool* Instance()
	{
		static MemPool pool;
		return &pool;
	}
};

SymplNamespaceEnd
