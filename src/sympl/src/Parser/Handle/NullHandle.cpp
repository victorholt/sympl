//
// GameSencha, LLC 5/31/22.
//
#include <sympl/include/Parser/Handle/NullHandle.hpp>
SymplNamespace

void NullHandle::__Construct(int argc, va_list ArgList)
{
    ValueHandle::__Construct(argc, ArgList);

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

SharedPtr<ValueHandle> NullHandle::Copy() const
{
	// We don't copy this particular handle.
	return ValueHandle::Null();
}

CStrPtr NullHandle::ToString() {
	StringRep->Set("null");
	return StringRep->CStr();
}
