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

	Bytes = static_cast<StrPtr>(calloc(1, Size));
	BlockSize = Size;
	Active = true;
	Static = false;

	Clear();
}

void MemBlock::Clear()
{
	if (!Bytes) {
		return;
	}

	memset(Bytes, 0, BlockSize);
	memset(Identifier, 0, sizeof(Identifier));
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
	Static = false;
}

void MemBlock::SetIdentifier(CStrPtr pIdentifier)
{
	memset(Identifier, 0, sizeof(Identifier));
	memcpy(Identifier, pIdentifier, sizeof(Identifier));
}
