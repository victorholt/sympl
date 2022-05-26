//
// GameSencha, LLC 5/25/22.
//
#include "MemPool.hpp"
#include "MemBlock.hpp"
#include "ManagedObject.hpp"
SymplNamespace

MemBlock* MemPool::CreateBlock(size_t BlockSize)
{
	MemBlock* Block = FindValidInactiveBlock(BlockSize);

	if (!Block) {
		Block = new MemBlock();
		Block->Create(BlockSize);

		Blocks.emplace_back(Block);
		Block->BlockIndex = Blocks.size();
	}

	return Block;
}

void MemPool::FreeBlock(size_t Index)
{
	if (Index <= 0 || Index >= Blocks.size()) {
		return;
	}

	MemBlock* Block = Blocks[Block->BlockIndex];
	Block->Active = false;

	auto Object = reinterpret_cast<ManagedObject*>(Block->Bytes);
	Object->Release();
}

void MemPool::Clear()
{
	for (auto& Block : Blocks)
	{
		Block->Free();
		delete Block;
	}

	Blocks.clear();
}

size_t MemPool::TotalMemUsage() const
{
	size_t Total = 0;
	for (const auto& Block : Blocks)
	{
		Total += Block->BlockSize;
	}

	return Total;
}

MemBlock* MemPool::FindValidInactiveBlock(size_t MinBlockSize)
{
	MemBlock* ValidBlock = nullptr;

	for (const auto& Block : Blocks)
	{
		if (Block->Active) {
			continue;
		}

		if (!ValidBlock) {
			ValidBlock = Block;
			continue;
		}

		if (MinBlockSize > 0) {
			if (Block->BlockSize > MinBlockSize && ValidBlock->BlockSize > Block->BlockSize) {
				ValidBlock = Block;
			}
		}
	}

	return ValidBlock;
}
