//
// Created by Victor on 5/25/2022.
//
#include "FloatHandle.hpp"
#include <sympl/Parser/LexerPosition.hpp>
#include <sympl/Parser/Error/RuntimeError.hpp>
#include <fmt/format.h>
SymplNamespace

FloatHandle::FloatHandle()
{
    Type = ValueType::Float;
    SetPosition(nullptr, nullptr);
}

SharedPtr<ValueHandle> FloatHandle::AddTo(const SharedPtr<ValueHandle>& handle)
{
    auto NewNumber = FloatHandle::Alloc<FloatHandle>();
    NewNumber->Context = Context;
    NewNumber->Value.FloatNum = Value.FloatNum + handle->Value.FloatNum;
    return NewNumber.Ptr();
}

SharedPtr<ValueHandle> FloatHandle::SubtractBy(const SharedPtr<ValueHandle>& handle)
{
    auto NewNumber = FloatHandle::Alloc<FloatHandle>();
    NewNumber->Context = Context;
    NewNumber->Value.FloatNum = Value.FloatNum - handle->Value.FloatNum;
    return NewNumber.Ptr();
}

SharedPtr<ValueHandle> FloatHandle::MultiplyBy(const SharedPtr<ValueHandle>& handle)
{
    auto NewNumber = FloatHandle::Alloc<FloatHandle>();
    NewNumber->Context = Context;
    NewNumber->Value.FloatNum = Value.FloatNum * handle->Value.FloatNum;
    return NewNumber.Ptr();
}
SharedPtr<ValueHandle> FloatHandle::DivideBy(const SharedPtr<ValueHandle>& handle)
{
    auto NewNumber = FloatHandle::Alloc<FloatHandle>();
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

    NewNumber->Value.FloatNum = Value.FloatNum / handle->Value.FloatNum;
    return NewNumber.Ptr();
}

SharedPtr<ValueHandle> FloatHandle::PowerBy(const SharedPtr<ValueHandle>& handle)
{
    auto NewNumber = FloatHandle::Alloc<FloatHandle>();
    NewNumber->Context = Context;
    NewNumber->Value.FloatNum = static_cast<float>(std::pow(Value.IntNum, handle->Value.IntNum));
    return NewNumber.Ptr();
}

SharedPtr<ValueHandle> FloatHandle::Copy() const
{
    auto NewValue = ValueHandle::BaseCopy<FloatHandle>(this);
    NewValue->Value = Value;
    return dynamic_cast<ValueHandle*>(NewValue.Ptr());
}

CStrPtr FloatHandle::ToString()
{
    memset(TmpNumber_Allocation, 0, sizeof(TmpNumber_Allocation));
    strcpy(TmpNumber_Allocation, fmt::format("{0}", Value.FloatNum).c_str());
    return TmpNumber_Allocation;
}
