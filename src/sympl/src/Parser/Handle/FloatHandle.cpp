//
// Created by Victor on 5/25/2022.
//
#include <sympl/include/Parser/Handle/FloatHandle.hpp>
#include <sympl/include/Parser/LexerPosition.hpp>
#include <sympl/include/Parser/Error/RuntimeError.hpp>
#include <fmt/format.h>
SymplNamespace

void FloatHandle::__Construct(int argc, va_list ArgList)
{
    ValueHandle::__Construct(argc, ArgList);

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
	StringRep->Set(fmt::format("{0}", Value.FloatNum).c_str());
    return StringRep->CStr();
}
