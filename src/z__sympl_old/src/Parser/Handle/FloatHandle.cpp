//
// Created by Victor on 5/25/2022.
//
#include <__sympl_old/include/Parser/Handle/FloatHandle.hpp>
#include <__sympl_old/include/Parser/LexerPosition.hpp>
#include <__sympl_old/include/Parser/Error/RuntimeError.hpp>
#include <__sympl_old/include/Parser/ParserContext.hpp>
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
                Context.Ptr(),
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
