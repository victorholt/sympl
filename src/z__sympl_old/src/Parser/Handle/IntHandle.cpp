//
// Created by Victor on 5/25/2022.
//
#include <__sympl_old/include/Parser/Handle/IntHandle.hpp>
#include <__sympl_old/include/Parser/LexerPosition.hpp>
#include <__sympl_old/include/Parser/Error/RuntimeError.hpp>
#include <__sympl_old/include/Parser/ParserContext.hpp>
#include <fmt/format.h>
SymplNamespace

void IntHandle::__Construct(int argc, va_list Args)
{
    ValueHandle::__Construct(argc, Args);

    Type = ValueType::Int;
    SetPosition(nullptr, nullptr);
}

void IntHandle::__Destruct()
{
    ValueHandle::__Destruct();
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
            Context.Ptr(),
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
//    if (Block->Static) {
//        SharedPtr<ValueHandle> Self = const_cast<IntHandle*>(this);
//        return Self;
//    }

    auto NewValue = ValueHandle::BaseCopy<IntHandle>(this);
    NewValue->Value = Value;
    return ObjectRef::CastTo<ValueHandle>(NewValue.Ptr());
}

CStrPtr IntHandle::ToString()
{
	StringRep->Set(fmt::format("{0}", Value.IntNum).c_str());
    return StringRep->CStr();
}
