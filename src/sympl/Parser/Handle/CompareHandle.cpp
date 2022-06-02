//
// GameSencha, LLC 5/31/22.
//
#include "CompareHandle.hpp"
#include "NumberHandle.hpp"
SymplNamespace

void CompareHandle::__Construct(int argc, va_list ArgList)
{
	ValueType pType = ValueType::Int;
	SharedPtr<ValueHandle> FromValue;

	if (argc > 0) {
		pType = va_arg(ArgList, ValueType);

        if (argc > 1) {
            FromValue = va_arg(ArgList, ValueHandle*);
        }
	}

	Create(pType, FromValue);
}

void CompareHandle::Create(ValueType pType, SharedPtr<ValueHandle> FromValue)
{
	Type = pType;

	// Copy over the value to compare.
	if (FromValue.IsValid()) {
		Value = FromValue->Value;

		if (FromValue->GetTypeName() == "CompareHandle")
		{
			Result = dynamic_cast<CompareHandle*>(FromValue.Ptr())->Result;
		}
	}
}

SharedPtr<CompareHandle> CompareHandle::CreateFrom(const SharedPtr<ValueHandle> Handle, const SharedPtr<ParserContext>& pContext)
{
	auto Result = CompareHandle::Alloc<CompareHandle>(2, Handle->Type, Handle.Ptr());
    Result->Context = pContext;
    return Result;
}

SharedPtr<CompareHandle> CompareHandle::CompareEqual(const SharedPtr<CompareHandle> &handle)
{
	auto NewCompare = CompareHandle::Alloc<CompareHandle>(1, handle->Type);
	NewCompare->Context = Context;
	NormalizeValue();

	switch (handle->Type)
	{
		case ValueType::String: {
			NewCompare->Result = strcmp(Value.String->CStr(), handle->Value.String->CStr()) == 0;
			break;
		}
		case ValueType::Int: {
			NewCompare->Result = Value.IntNum == handle->Value.IntNum;
			break;
		}
		case ValueType::Float: {
			NewCompare->Result = Value.FloatNum == handle->Value.FloatNum;
			break;
		}
	}

	return NewCompare.Ptr();
}

SharedPtr<CompareHandle> CompareHandle::CompareNotEqual(const SharedPtr<CompareHandle> &handle)
{
	auto NewCompare = CompareHandle::Alloc<CompareHandle>(1, handle->Type);
	NewCompare->Context = Context;
	NormalizeValue();

	switch (handle->Type)
	{
		case ValueType::String: {
			NewCompare->Result = strcmp(Value.String->CStr(), handle->Value.String->CStr()) != 0;
			break;
		}
		case ValueType::Int: {
			NewCompare->Result = Value.IntNum != handle->Value.IntNum;
			break;
		}
		case ValueType::Float: {
			NewCompare->Result = Value.FloatNum != handle->Value.FloatNum;
			break;
		}
	}

	return NewCompare.Ptr();
}

SharedPtr<CompareHandle> CompareHandle::CompareLessThan(const SharedPtr<CompareHandle> &handle)
{
	auto NewCompare = CompareHandle::Alloc<CompareHandle>(1, handle->Type);
	NewCompare->Context = Context;
	NormalizeValue();

	switch (handle->Type)
	{
		case ValueType::String: {
			NewCompare->Result = strcmp(Value.String->CStr(), handle->Value.String->CStr()) < 0;
			break;
		}
		case ValueType::Int: {
			NewCompare->Result = Value.IntNum < handle->Value.IntNum;
			break;
		}
		case ValueType::Float: {
			NewCompare->Result = Value.FloatNum < handle->Value.FloatNum;
			break;
		}
	}

	return NewCompare.Ptr();
}

SharedPtr<CompareHandle> CompareHandle::CompareLessThanOrEqual(const SharedPtr<CompareHandle> &handle)
{
	auto NewCompare = CompareHandle::Alloc<CompareHandle>(1, handle->Type);
	NewCompare->Context = Context;
	NormalizeValue();

	switch (handle->Type)
	{
		case ValueType::String: {
			NewCompare->Result = strcmp(Value.String->CStr(), handle->Value.String->CStr()) <= 0;
			break;
		}
		case ValueType::Int: {
			NewCompare->Result = Value.IntNum <= handle->Value.IntNum;
			break;
		}
		case ValueType::Float: {
			NewCompare->Result = Value.FloatNum <= handle->Value.FloatNum;
			break;
		}
	}

	return NewCompare.Ptr();
}

SharedPtr<CompareHandle> CompareHandle::CompareGreaterThan(const SharedPtr<CompareHandle> &handle)
{
	auto NewCompare = CompareHandle::Alloc<CompareHandle>(1, handle->Type);
	NewCompare->Context = Context;
	NormalizeValue();

	switch (handle->Type)
	{
		case ValueType::String: {
			NewCompare->Result = strcmp(Value.String->CStr(), handle->Value.String->CStr()) > 0;
			break;
		}
		case ValueType::Int: {
			NewCompare->Result = Value.IntNum > handle->Value.IntNum;
			break;
		}
		case ValueType::Float: {
			NewCompare->Result = Value.FloatNum > handle->Value.FloatNum;
			break;
		}
	}

	return NewCompare.Ptr();
}

SharedPtr<CompareHandle> CompareHandle::CompareGreaterThanOrEqual(const SharedPtr<CompareHandle> &handle)
{
	auto NewCompare = CompareHandle::Alloc<CompareHandle>(1, handle->Type);
	NewCompare->Context = Context;
	NormalizeValue();

	switch (handle->Type)
	{
		case ValueType::String: {
			NewCompare->Result = strcmp(Value.String->CStr(), handle->Value.String->CStr()) >= 0;
			break;
		}
		case ValueType::Int: {
			NewCompare->Result = Value.IntNum >= handle->Value.IntNum;
			break;
		}
		case ValueType::Float: {
			NewCompare->Result = Value.FloatNum >= handle->Value.FloatNum;
			break;
		}
	}

	return NewCompare.Ptr();
}

SharedPtr<CompareHandle> CompareHandle::CompareNot()
{
	auto NewCompare = CompareHandle::Alloc<CompareHandle>(1, Type);
	NewCompare->Context = Context;
	NormalizeValue();

	switch (Type)
	{
		case ValueType::String: {
			NewCompare->Result = !Value.String.IsValid();
			break;
		}
		case ValueType::Int: {
			NewCompare->Result = !Value.IntNum;
			break;
		}
		case ValueType::Float: {
			NewCompare->Result = !static_cast<int>(Value.FloatNum);
			break;
		}
		case ValueType::Null: {
			NewCompare->Result = !Value.String.IsValid() && !Value.IntNum && !static_cast<int>(Value.FloatNum);
			break;
		}
	}

	return NewCompare.Ptr();
}

SharedPtr<CompareHandle> CompareHandle::CompareAnd(const SharedPtr<CompareHandle> &handle)
{
	auto NewCompare = CompareHandle::Alloc<CompareHandle>(1, handle->Type);
	NewCompare->Context = Context;
	NormalizeValue();

	NewCompare->Result = Result && handle->Result;

	return NewCompare.Ptr();
}

SharedPtr<CompareHandle> CompareHandle::CompareOr(const SharedPtr<CompareHandle> &handle)
{
	auto NewCompare = CompareHandle::Alloc<CompareHandle>(1, handle->Type);
	NewCompare->Context = Context;
	NormalizeValue();

	NewCompare->Result = Result || handle->Result;

	return NewCompare.Ptr();
}

bool CompareHandle::IsTrue() const
{
	return Result;
}

CStrPtr CompareHandle::ToString() {
	memset(TmpNumber_Allocation, 0, sizeof(TmpNumber_Allocation));
	strcpy(TmpNumber_Allocation, fmt::format("{0}", Result ? "true" : "false").c_str());
	return TmpNumber_Allocation;
}
