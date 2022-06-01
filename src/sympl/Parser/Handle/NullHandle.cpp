//
// GameSencha, LLC 5/31/22.
//
#include "NullHandle.hpp"
SymplNamespace

void NullHandle::__Construct(int argc, va_list ArgList)
{
	Create();
}

void NullHandle::Create()
{
	Type = ValueType::Null;
}

bool NullHandle::IsTrue() const
{
	return false;
}

CStrPtr NullHandle::ToString() {
	memset(TmpNumber_Allocation, 0, sizeof(TmpNumber_Allocation));
	strcpy(TmpNumber_Allocation, "null");
	return TmpNumber_Allocation;
}
