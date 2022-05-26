//
// GameSencha, LLC 5/25/22.
//
#include "ManagedObject.hpp"
SymplNamespace

ManagedObject::ManagedObject()
{
    Block = nullptr;
}

ManagedObject::~ManagedObject()
{
}

void ManagedObject::__Construct(int argc, va_list ArgList)
{
}

int ManagedObject::Release()
{
    auto Result = SharedPtrRef::Release();

	if (Result == 0 && Block && Block->BlockIndex > 0)
	{
		MemPool::Instance()->FreeBlock(Block->BlockIndex);
        Block = nullptr;
	}

    return Result;
}


