//
// Created by Victor on 5/25/2022.
//
#include <sympl/include/Parser/Handle/IntHandle.hpp>
#include <sympl/include/Parser/LexerPosition.hpp>
#include <sympl/include/Parser/Error/RuntimeError.hpp>
#include <fmt/format.h>
SymplNamespace

IntHandle::IntHandle()
{
    Type = ValueType::Int;
    SetPosition(nullptr, nullptr);
}

SharedPtr<ValueHandle> IntHandle::AddTo(const SharedPtr<ValueHandle>& handle)
{
    auto NewNumber = IntHandle::Alloc<IntHandle>();
    NewNumber->Context = Context;
    NewNumber->Value.IntNum = Value.IntNum + handle->Value.IntNum;
    return NewNumber.Ptr();
}

SharedPtr<ValueHandle> IntHandle::SubtractBy(const SharedPtr<ValueHandle>& handle)
{
    auto NewNumber = IntHandle::Alloc<IntHandle>();
    NewNumber->Context = Context;
    NewNumber->Value.IntNum = Value.IntNum - handle->Value.IntNum;
    return NewNumber.Ptr();
}

SharedPtr<ValueHandle> IntHandle::MultiplyBy(const SharedPtr<ValueHandle>& handle)
{
    auto NewNumber = IntHandle::Alloc<IntHandle>();
    NewNumber->Context = Context;
    NewNumber->Value.IntNum = Value.IntNum * handle->Value.IntNum;
    return NewNumber.Ptr();
}
SharedPtr<ValueHandle> IntHandle::DivideBy(const SharedPtr<ValueHandle>& handle)
{
    auto NewNumber = IntHandle::Alloc<IntHandle>();
    NewNumber->Context = Context;

    if (handle->Value.IntNum == 0) {
        NewNumber->Error = SharedPtr<RuntimeError>(new RuntimeError(
            Context,
            StartPosition,
            EndPosition,
            "Division by zero"
        )).Ptr();
        return NewNumber.Ptr();
    }

    NewNumber->Value.IntNum = Value.IntNum / handle->Value.IntNum;
    return NewNumber.Ptr();
}

SharedPtr<ValueHandle> IntHandle::PowerBy(const SharedPtr<ValueHandle>& handle)
{
    auto NewNumber = IntHandle::Alloc<IntHandle>();
    NewNumber->Context = Context;
    NewNumber->Value.IntNum = static_cast<long>(std::pow(Value.IntNum, handle->Value.IntNum));
    return NewNumber.Ptr();
}

SharedPtr<ValueHandle> IntHandle::Copy() const
{
    auto NewValue = ValueHandle::BaseCopy<IntHandle>(this);
    NewValue->Value = Value;
    return dynamic_cast<ValueHandle*>(NewValue.Ptr());
}

CStrPtr IntHandle::ToString()
{
	StringRep->Set(fmt::format("{0}", Value.IntNum).c_str());
    return StringRep->CStr();
}
