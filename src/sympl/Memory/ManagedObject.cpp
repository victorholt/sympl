//
// GameSencha, LLC 5/25/22.
//
#include "ManagedObject.hpp"
SymplNamespace

ManagedObject::ManagedObject()
{
}

ManagedObject::~ManagedObject()
{
	Release();
}

void ManagedObject::Release()
{
	if (Block)
	{
		MemPool::Instance()->FreeBlock(Block->BlockIndex);
	}
}


