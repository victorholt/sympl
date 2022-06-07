//
// GameSencha, LLC 5/25/22.
//
#include <sympl/include/Memory/MemBlock.hpp>
SymplNamespace

MemBlock::MemBlock()
{
	Bytes = nullptr;
	BlockIndex = -1;
	Active = false;
    Static = false;
}

void MemBlock::Create(size_t Size)
{
	// Delete any previous bytes.
	delete [] Bytes;

	Bytes = static_cast<StrPtr>(calloc(1, Size + 1));
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
