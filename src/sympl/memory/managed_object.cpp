//
// GameSencha, LLC 1/31/24.
//
#include "managed_object.hpp"
SymplNamespace

ManagedObject::ManagedObject()
{
    mem_block = nullptr;
}

ManagedObject::~ManagedObject()
{
}

void ManagedObject::__construct(int argc, va_list ArgList)
{
}

int ManagedObject::release()
{
    if (mem_block && mem_block->is_static) {
        return 1;
    }

    auto Result = ObjectRef::release();
    assert(Result >= 0);

    if (Result == 0 && mem_block && mem_block->block_index > 0)
    {
        __destruct();
        MemPool::instance()->free_block(mem_block);
        mem_block = nullptr;
    }

    return Result;
}

ManagedObject* ManagedObject::mem_copy()
{
    auto mem_data = static_cast<ManagedObject*>(malloc(object_size));
    memcpy(static_cast<void*>(mem_data), static_cast<void*>(this), object_size);
    return mem_data;
}