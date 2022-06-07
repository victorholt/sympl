//
// GameSencha, LLC 5/25/22.
//
#include <sympl/include/Memory/MemPool.hpp>
#include <sympl/include/Memory/MemBlock.hpp>
#include <sympl/include/Memory/ManagedObject.hpp>
SymplNamespace

MemPool::MemPool()
{
    AllocBlock(2000);
}

void MemPool::AllocBlock(int NumBlocks)
{
    for (int i = 0; i < NumBlocks; ++i) {
        CreateBlock(DefaultBlockSize);
    }
}

MemBlock* MemPool::CreateBlock(size_t BlockSize)
{
	MemBlock* Block = FindValidInactiveBlock(BlockSize);

	if (!Block) {
		Block = new MemBlock();
		Block->Create(BlockSize);

        Block->BlockIndex = Blocks.size();
        Blocks.emplace_back(Block);
	}

	return Block;
}

void MemPool::FreeBlock(MemBlock* pBlock)
{
    if (!pBlock) {
        return;
    }

	if (pBlock->BlockIndex <= 0 || pBlock->BlockIndex >= Blocks.size()) {
		return;
	}

    MemBlock* Block = Blocks[pBlock->BlockIndex];
	Block->Active = false;

//	auto Object = reinterpret_cast<ManagedObject*>(Block->Bytes);
//	Object->Release();
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

bool MemPool::IsMaxMemUsage() const
{
	return false;
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
