//
// GameSencha, LLC 5/25/22.
//
#include <sympl/include/Memory/MemPool.hpp>
#include <sympl/include/Memory/MemBlock.hpp>
#include <sympl/include/Memory/ManagedObject.hpp>
SymplNamespace

MemPool::MemPool()
{
    DefaultBlockSize = 256;
    AllocBlocks(2000);
}

void MemPool::AllocBlocks(int NumBlocks)
{
    for (int i = 0; i < NumBlocks; ++i) {
        auto Block = CreateBlock(DefaultBlockSize);
		Block->Active = false;
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

	assert(Block->BlockSize >= BlockSize);

	Block->Clear();
	Block->Active = true;
	return Block;
}

void MemPool::FreeBlock(MemBlock* pBlock)
{
    if (!pBlock || pBlock->Static) {
        return;
    }

	assert(pBlock->BlockIndex >= 0 && pBlock->BlockIndex < Blocks.size());

    MemBlock* Block = Blocks[pBlock->BlockIndex];
	Block->Active = false;
	Block->Static = false;
    reinterpret_cast<ManagedObject*>(Block->Bytes)->~ManagedObject();
    Block->Clear();
}

void MemPool::FreeAllBlocks()
{
	for (auto& Block : Blocks)
	{
		Block->Active = false;
		Block->Static = false;
        Block->Clear();
	}
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

		if (Block->BlockSize > MinBlockSize) {
			if (!ValidBlock) {
				ValidBlock = Block;
				continue;
			}

			// Choose the smallest active block.
			if (ValidBlock->BlockSize > Block->BlockSize) {
				ValidBlock = Block;
			}
		}
	}

	// Ensure the block is cleaned.
	return ValidBlock;
}

size_t MemPool::GetMemUsage() const
{
	size_t Result = 0;
	for (const auto& Block : Blocks) {
		if (!Block->Active) {
			continue;
		}
		Result += Block->BlockSize;
	}
	return Result;
}

size_t MemPool::GetUsedBlocks() const
{
	size_t Result = 0;
	for (const auto& Block : Blocks) {
		if (!Block->Active) {
			continue;
		}
		Result++;
	}
	return Result;
}

size_t MemPool::GetUnusedBlocks() const
{
	size_t Result = 0;
	for (const auto& Block : Blocks) {
		if (Block->Active) {
			continue;
		}
		Result++;
	}
	return Result;
}

void MemPool::GetUsedBlockObjectNames(std::vector<std::string> &Output)
{
	std::unordered_map<std::string, int> BlockMapCount;

	for (const auto& Block : Blocks) {
		if (!Block->Active) {
			continue;
		}

		auto Entry = BlockMapCount.find(Block->GetIndenifier());
		if (Entry == BlockMapCount.end()) {
			BlockMapCount[Block->GetIndenifier()] = 1;
			continue;
		}

		Entry->second++;
	}

	for (const auto& Entry : BlockMapCount) {
		Output.emplace_back(fmt::format("{0}: {1}", Entry.first, Entry.second));
	}
}
