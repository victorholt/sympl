//
// GameSencha, LLC 5/31/22.
//
#include "ListHandle.hpp"
#include "NullHandle.hpp"
#include <sympl/Parser/Node/ParserNode.hpp>
SymplNamespace

void ListHandle::__Construct(int argc, va_list ArgList)
{
}

void ListHandle::Create(const std::vector<SharedPtr<ParserNode>>& pElements)
{
    Type = ValueType::String;
    Elements = pElements;
}

SharedPtr<ValueHandle> ListHandle::AddTo(const SharedPtr <ValueHandle>& pHandle)
{
    if (pHandle->Type != ValueType::String) {
        return NullHandle::Alloc<NullHandle>().Ptr();
    }

    auto NewStr = StringBuffer::Alloc<StringBuffer>();
    NewStr->Set(Value.String->CStr());
    NewStr->Append(pHandle->Value.String.Ptr());

    auto Result = ListHandle::Alloc<ListHandle>(1, NewStr->CStr());
    Result->Context = Context;

    return Result.Ptr();
}

SharedPtr<ValueHandle> ListHandle::MultiplyBy(const SharedPtr<class ValueHandle>& pHandle)
{
    if (pHandle->Type != ValueType::Int) {
        return NullHandle::Alloc<NullHandle>().Ptr();
    }

    auto NewStr = StringBuffer::Alloc<StringBuffer>();
    for (auto i = 0; i < pHandle->Value.IntNum; ++i) {
        NewStr->Append(Value.String.Ptr());
    }

    auto Result = ListHandle::Alloc<ListHandle>(1, NewStr->CStr());
    Result->Context = Context;

    return Result.Ptr();
}

void ListHandle::NormalizeValue()
{
    Value.IntNum = Value.String.IsValid() && Value.String->Length() > 0 ? 1 : 0;
    Value.FloatNum = Value.String.IsValid() && Value.String->Length() > 0  ? 1.f : 0.f;
}

bool ListHandle::IsTrue() const
{
    return Value.String.IsValid() && Value.String->Length() > 0;
}

SharedPtr<ValueHandle> ListHandle::Copy() const
{
    auto Result = ListHandle::Alloc<ListHandle>(1, Value.String->CStr());
    Result->NormalizeValue();
    Result->SetPosition(StartPosition, EndPosition);
    Result->Context = Context;
    return Result.Ptr();
}

CStrPtr ListHandle::ToString() {
    SharedPtr<StringBuffer> Result = StringBuffer::Alloc<StringBuffer>();
    Result->Append("<list>[");

    for (const auto& Item : Elements) {

    }
    return Value.String->CStr();
}
