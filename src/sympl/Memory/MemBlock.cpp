//
// GameSencha, LLC 5/25/22.
//
#include "MemBlock.hpp"
SymplNamespace

MemBlock::MemBlock()
{
	Bytes = nullptr;
	BlockIndex = -1;
	Active = false;
}

void MemBlock::Create(size_t Size)
{
	// Delete any previous bytes.
	delete [] Bytes;

	Bytes = static_cast<StrPtr>(malloc(Size));
	BlockSize = Size;
	Active = true;

	Clear();
}

void MemBlock::Clear()
{
	if (!Bytes) {
		return;
	}

	memset(Bytes, 0, BlockSize);
}

void MemBlock::Free()
{
	if (!Bytes) {
		return;
	}

	free(Bytes);

	Bytes = nullptr;
	BlockSize = 0;
	BlockIndex = -1;
	Active = false;
}
