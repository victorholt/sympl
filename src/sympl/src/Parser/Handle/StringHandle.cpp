//
// GameSencha, LLC 5/31/22.
//
#include <sympl/include/Parser/Handle/StringHandle.hpp>
#include <sympl/include/Parser/Handle/ExceptionHandle.hpp>
#include <sympl/include/Parser/Handle/NullHandle.hpp>
#include <sympl/include/Parser/Error/RuntimeError.hpp>
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
        return ValueHandle::Null();
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
        return ValueHandle::Null();
    }

    auto NewStr = StringBuffer::Alloc<StringBuffer>();
    for (auto i = 0; i < pHandle->Value.IntNum; ++i) {
        NewStr->Append(Value.String.Ptr());
    }

    auto Result = StringHandle::Alloc<StringHandle>(1, NewStr->CStr());
    Result->Context = Context;

    return Result.Ptr();
}

SharedPtr<ValueHandle> StringHandle::DivideBy(const SharedPtr<ValueHandle>& pHandle)
{
    if (pHandle->Type != ValueType::Int) {
        return ValueHandle::Null();
    }

    auto Index = pHandle->Value.IntNum;
    if (Index >= Value.String->Length() || Index < 0) {
        auto Error = SharedPtr<RuntimeError>(new RuntimeError(
                Context,
                StartPosition,
                EndPosition,
                fmt::format("Character at index '{0}' in <string> could not be retrieved. Index out of bounds.", Index).c_str()
        ));
        return ExceptionHandle::Alloc<ExceptionHandle>(1, Error.Ptr()).Ptr();
    }

    std::string str(1, Value.String->Get(Index));
    auto Result = StringHandle::Alloc<StringHandle>(1, str.c_str());
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
