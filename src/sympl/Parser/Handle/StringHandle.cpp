//
// GameSencha, LLC 5/31/22.
//
#include "StringHandle.hpp"
#include "NullHandle.hpp"
SymplNamespace

void StringHandle::__Construct(int argc, va_list ArgList)
{
    CStrPtr pValue = va_arg(ArgList, CStrPtr);
    Create(pValue);
}

void StringHandle::Create(CStrPtr pValue)
{
    Type = ValueType::String;
    Value.String = StringBuffer::Alloc<StringBuffer>();
    Value.String->Set(pValue);
}

SharedPtr<ValueHandle> StringHandle::AddTo(const SharedPtr <ValueHandle>& pHandle)
{
    if (pHandle->Type != ValueType::String) {
        return NullHandle::Alloc<NullHandle>().Ptr();
    }

    auto NewStr = StringBuffer::Alloc<StringBuffer>();
    NewStr->Set(Value.String->CStr());
    NewStr->Append(pHandle->Value.String.Ptr());

    auto Result = StringHandle::Alloc<StringHandle>(1, NewStr->CStr());
    Result->Context = Context;

    return Result.Ptr();
}

SharedPtr<ValueHandle> StringHandle::MultiplyBy(const SharedPtr<class ValueHandle>& pHandle)
{
    if (pHandle->Type != ValueType::Int) {
        return NullHandle::Alloc<NullHandle>().Ptr();
    }

    auto NewStr = StringBuffer::Alloc<StringBuffer>();
    for (auto i = 0; i < pHandle->Value.IntNum; ++i) {
        NewStr->Append(Value.String.Ptr());
    }

    auto Result = StringHandle::Alloc<StringHandle>(1, NewStr->CStr());
    Result->Context = Context;

    return Result.Ptr();
}

void StringHandle::NormalizeValue()
{
    Value.IntNum = Value.String.IsValid() && Value.String->Length() > 0 ? 1 : 0;
    Value.FloatNum = Value.String.IsValid() && Value.String->Length() > 0  ? 1.f : 0.f;
}

bool StringHandle::IsTrue() const
{
    return Value.String.IsValid() && Value.String->Length() > 0;
}

SharedPtr<ValueHandle> StringHandle::Copy() const
{
    auto Result = StringHandle::Alloc<StringHandle>(1, Value.String->CStr());
    Result->NormalizeValue();
    Result->SetPosition(StartPosition, EndPosition);
    Result->Context = Context;
    return Result.Ptr();
}

CStrPtr StringHandle::ToString() {
    return Value.String->CStr();
}
