//
// GameSencha, LLC 5/25/22.
//
#pragma once
#include <__sympl_old/SymplPCH.hpp>

SymplNamespaceStart

class MemBlock
{
protected:
	// Identifier used for debugging.
	char Identifier[256];

public:
	// Memory block index.
	size_t BlockIndex = 0;

	// Current size of the block.
	size_t BlockSize;

	// Max size of the block.
	bool Active;

	// Check if this block can be changed.
	bool Static;

	// Memory of the block.
	StrPtr Bytes;

	// Constructor.
	MemBlock();

	// Creates the block with the given size.
	void Create(size_t Size);

	// Clears out the block memory.
	void Clear();

	// Frees the memory.
	void Free();

	// Sets the identifier.
	void SetIdentifier(CStrPtr pIdentifier);

	/**
	 * Returns the identifier.
	 * @return
	 */
	inline const char* GetIndenifier() const { return Identifier; }
};

SymplNamespaceEnd
