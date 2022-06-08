//
// Created by Victor on 5/26/2022.
//
#include <sympl/include/Parser/Handle/ValueHandle.hpp>
#include <sympl/include/Parser/Handle/IntHandle.hpp>
#include <sympl/include/Parser/Handle/NullHandle.hpp>
#include <sympl/include/Parser/Handle/ExceptionHandle.hpp>
#include <sympl/include/Parser/ParserContext.hpp>
#include <sympl/include/Parser/LexerPosition.hpp>
#include <sympl/include/Parser/ParserRuntimeResult.hpp>
#include <sympl/include/Parser/SymbolTable.hpp>
#include <sympl/include/Parser/Error/RuntimeError.hpp>
#include <sympl/include/Parser/ParserContext.hpp>
SymplNamespace

SharedPtr<NullHandle> ValueHandle::NullValue;
SharedPtr<IntHandle> ValueHandle::TrueValue;
SharedPtr<IntHandle> ValueHandle::FalseValue;

void ValueHandle::__Construct(int argc, va_list Args)
{
    Type = ValueType::Null;
    SetPosition(LexerPosition::Alloc<LexerPosition>(), LexerPosition::Alloc<LexerPosition>());
    Error = nullptr;
    Context = nullptr;
	StringRep = StringBuffer::Alloc<StringBuffer>();
}

void ValueHandle::__Destruct()
{
    Value.String = nullptr;
    StringRep = nullptr;
    Error = nullptr;
    StartPosition = nullptr;
    EndPosition = nullptr;
    Context = nullptr;
}

void ValueHandle::SetPosition(const SharedPtr<LexerPosition>& pStartPosition, const SharedPtr<LexerPosition>& pEndPosition)
{
    StartPosition = pStartPosition;
    EndPosition = pEndPosition;
}

SharedPtr<ValueHandle> ValueHandle::AddTo(const SharedPtr<ValueHandle>& pHandle)
{
	return ValueHandle::GetIllegalOperationException(this);
}

SharedPtr<ValueHandle> ValueHandle::SubtractBy(const SharedPtr<ValueHandle>& pHandle)
{
	return ValueHandle::GetIllegalOperationException(this);
}

SharedPtr<ValueHandle> ValueHandle::MultiplyBy(const SharedPtr<ValueHandle>& pHandle)
{
	return ValueHandle::GetIllegalOperationException(this);
}

SharedPtr<ValueHandle> ValueHandle::DivideBy(const SharedPtr<ValueHandle>& pHandle)
{
	return ValueHandle::GetIllegalOperationException(this);
}

SharedPtr<ValueHandle> ValueHandle::PowerBy(const SharedPtr<ValueHandle>& pHandle)
{
	return ValueHandle::GetIllegalOperationException(this);
}

SharedPtr<ParserContext> ValueHandle::GenerateNewContext(
    CStrPtr ContextName,
    const SharedPtr<ParserContext>& pParentContext
) const
{
    SharedPtr<ParserContext> ParentContext = pParentContext.IsValid() ? pParentContext : Context.Ptr();

    auto NewContext = ParserContext::Alloc<ParserContext>();
    NewContext->Create(ParentContext, StartPosition, ContextName);

    if (ParentContext.IsValid() && ParentContext->VariableSymbolTable.IsValid()) {
        NewContext->VariableSymbolTable = SymbolTable::Alloc<SymbolTable>(1, ParentContext->VariableSymbolTable.Ptr());
    }
    else {
        NewContext->VariableSymbolTable = SymbolTable::Alloc<SymbolTable>();
    }

    return NewContext;
}

void ValueHandle::NormalizeValue()
{
	switch (Type)
	{
		case ValueType::Int:
			Value.FloatNum = static_cast<float>(Value.IntNum);
			break;
		case ValueType::Float:
			Value.IntNum = static_cast<int>(Value.FloatNum);
			break;
		case ValueType::Null:
			break;
	}
}

bool ValueHandle::IsTrue() const
{
	return Value.String.IsValid() && Value.IntNum != 0 && static_cast<int>(Value.FloatNum) != 0;
}

bool ValueHandle::IsNull() const {
	return Type == ValueType::Null && GetTypeName() == "NullHandle";
}

//SharedPtr<ValueHandle> ValueHandle::Copy() const
//{
//    return ValueHandle::BaseCopy<ValueHandle>(this);
//}

SharedPtr<ParserRuntimeResult> ValueHandle::Exec(const std::vector<SharedPtr<ValueHandle>>& ArgValueList)
{
	auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();
	Result->Error = SharedPtr<RuntimeError>(new RuntimeError(
		Context.Ptr(),
		StartPosition,
		EndPosition,
		fmt::format("Illegal operation attempt on executing {0}", ToString()).c_str()
	)).Ptr();
	return Result;
}

SharedPtr<ValueHandle> ValueHandle::GetIllegalOperationException(SharedPtr<ValueHandle> pValue)
{
	auto Exception = SharedPtr<RuntimeError>(new RuntimeError(
		pValue->Context.Ptr(),
		pValue->StartPosition->Copy(),
		pValue->EndPosition->Copy(),
		fmt::format("Illegal operation attempt on executing {0}", pValue->ToString()).c_str()
	));
	return ExceptionHandle::Alloc<ExceptionHandle>(1, Exception.Ptr()).Ptr();
}

SharedPtr<ValueHandle> ValueHandle::Null(ParserContext* Context)
{
    auto Value = NullHandle::Alloc<NullHandle>();
    Value->Context = Context;
    Value->Value.IntNum = 1;
    Value->Value.FloatNum = 1;
    Value->Immutable = true;
    return Value.Ptr();

//	static SharedPtr<NullHandle> NullValue;
//	SharedPtr<NullHandle> NullValue;
	if (!NullValue.IsValid()) {
        NullValue = NullHandle::Alloc<NullHandle>();
        NullValue->SetPosition(
            LexerPosition::Alloc<LexerPosition>(),
            LexerPosition::Alloc<LexerPosition>()
        );
        NullValue->Context = Context;
		NullValue->Value.IntNum = 0;
		NullValue->Value.FloatNum = 0;
		NullValue->Value.String = StringBuffer::Alloc<StringBuffer>();
        NullValue->Immutable = true;
		NullValue->Block->Static = true;
	}
	return NullValue.Ptr();
}

SharedPtr<ValueHandle> ValueHandle::True(ParserContext* Context)
{
    auto Value = IntHandle::Alloc<IntHandle>();
    Value->Context = Context;
    Value->Value.IntNum = 1;
    Value->Value.FloatNum = 1;
    Value->Immutable = true;
    return Value.Ptr();

//	static SharedPtr<IntHandle> TrueValue;
//	SharedPtr<IntHandle> TrueValue;
	if (!TrueValue.IsValid()) {
		TrueValue = IntHandle::Alloc<IntHandle>();
        TrueValue->SetPosition(
            LexerPosition::Alloc<LexerPosition>(),
            LexerPosition::Alloc<LexerPosition>()
        );
		TrueValue->Context = Context;
		TrueValue->Value.IntNum = 1;
		TrueValue->Value.FloatNum = 1;
		TrueValue->Value.String = StringBuffer::Alloc<StringBuffer>();
        TrueValue->Immutable = true;
		TrueValue->Block->Static = true;
	}
	return TrueValue.Ptr();
}

SharedPtr<ValueHandle> ValueHandle::False(ParserContext* Context)
{
    auto Value = IntHandle::Alloc<IntHandle>();
    Value->Context = Context;
    Value->Value.IntNum = 0;
    Value->Value.FloatNum = 0;
    Value->Immutable = true;
    return Value.Ptr();

//	static SharedPtr<IntHandle> FalseValue;
//	SharedPtr<IntHandle> FalseValue;
	if (!FalseValue.IsValid()) {
		FalseValue = IntHandle::Alloc<IntHandle>();
        FalseValue->SetPosition(
            LexerPosition::Alloc<LexerPosition>(),
            LexerPosition::Alloc<LexerPosition>()
        );
		FalseValue->Context = Context;
		FalseValue->Value.IntNum = 1;
		FalseValue->Value.FloatNum = 1;
		FalseValue->Value.String = StringBuffer::Alloc<StringBuffer>();
        FalseValue->Immutable = true;
		FalseValue->Block->Static = true;
	}
	return FalseValue.Ptr();
}

CStrPtr ValueHandle::ToString()
{
	StringRep->Set("value_handle_null");
	return StringRep->CStr();
}
