//
// GameSencha, LLC 5/25/22.
//
#include <sympl/include/Memory/ManagedObject.hpp>
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
	if (Block && Block->Static) {
		return 1;
	}

	auto Result = ObjectRef::Release();
	assert(Result >= 0);

	if (Result == 0 && Block && Block->BlockIndex > 0)
	{
		MemPool::Instance()->FreeBlock(Block);
        Block = nullptr;
	}

    return Result;
}

ManagedObject* ManagedObject::MemCopy()
{
    auto Data = static_cast<ManagedObject*>(malloc(ObjectSize));
    memcpy(static_cast<void*>(Data), static_cast<void*>(this), ObjectSize);
    return Data;
}
