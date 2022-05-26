//
// Created by Victor on 5/25/2022.
//
#include "IntHandle.hpp"
#include <sympl/Parser/LexerPosition.hpp>
#include <sympl/Parser/Error/RuntimeError.hpp>
#include <fmt/format.h>
SymplNamespace

IntHandle::IntHandle()
{
    Type = ValueType::Int;
    SetPosition(nullptr, nullptr);
}

SharedPtr<NumberHandle> IntHandle::AddTo(const SharedPtr<NumberHandle>& handle)
{
    auto NewNumber = IntHandle::Alloc<IntHandle>();
    NewNumber->Context = Context;
    NewNumber->Value.IntNum = Value.IntNum + handle->Value.IntNum;
    return NewNumber.Ptr();
}

SharedPtr<NumberHandle> IntHandle::SubtractBy(const SharedPtr<NumberHandle>& handle)
{
    auto NewNumber = IntHandle::Alloc<IntHandle>();
    NewNumber->Context = Context;
    NewNumber->Value.IntNum = Value.IntNum - handle->Value.IntNum;
    return NewNumber.Ptr();
}

SharedPtr<NumberHandle> IntHandle::MultiplyBy(const SharedPtr<NumberHandle>& handle)
{
    auto NewNumber = IntHandle::Alloc<IntHandle>();
    NewNumber->Context = Context;
    NewNumber->Value.IntNum = Value.IntNum * handle->Value.IntNum;
    return NewNumber.Ptr();
}
SharedPtr<NumberHandle> IntHandle::DivideBy(const SharedPtr<NumberHandle>& handle)
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

CStrPtr IntHandle::ToString()
{
    memset(TmpNumber_Allocation, 0, sizeof(TmpNumber_Allocation));
    strcpy(TmpNumber_Allocation, fmt::format("{0}", Value.IntNum).c_str());
    return TmpNumber_Allocation;
}
