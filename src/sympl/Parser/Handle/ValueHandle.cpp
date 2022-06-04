//
// Created by Victor on 5/26/2022.
//
#include "ValueHandle.hpp"
#include "IntHandle.hpp"
#include "NullHandle.hpp"
#include "ExceptionHandle.hpp"
#include <sympl/Parser/ParserContext.hpp>
#include <sympl/Parser/LexerPosition.hpp>
#include <sympl/Parser/ParserRuntimeResult.hpp>
#include <sympl/Parser/SymbolTable.hpp>
#include <sympl/Parser/Error/RuntimeError.hpp>
SymplNamespace

ValueHandle::ValueHandle()
{
    Type = ValueType::Null;
    SetPosition(LexerPosition::Alloc<LexerPosition>(), LexerPosition::Alloc<LexerPosition>());
    Error = nullptr;
    Context = nullptr;
	StringRep = StringBuffer::Alloc<StringBuffer>();
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

SharedPtr<ParserContext> ValueHandle::GenerateNewContext(CStrPtr ContextName) const
{
    auto NewContext = ParserContext::Alloc<ParserContext>();
    NewContext->Create(Context, StartPosition, ContextName);

    if (Context.IsValid() && Context->VariableSymbolTable.IsValid()) {
        NewContext->VariableSymbolTable = SymbolTable::Alloc<SymbolTable>(1, Context->VariableSymbolTable.Ptr());
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
		Context,
		StartPosition,
		EndPosition,
		fmt::format("Illegal operation attempt on executing {0}", ToString()).c_str()
	)).Ptr();
	return Result;
}

SharedPtr<ValueHandle> ValueHandle::GetIllegalOperationException(SharedPtr<ValueHandle> pValue)
{
	auto Exception = SharedPtr<RuntimeError>(new RuntimeError(
		pValue->Context,
		pValue->StartPosition,
		pValue->EndPosition,
		fmt::format("Illegal operation attempt on executing {0}", pValue->ToString()).c_str()
	));
	return ExceptionHandle::Alloc<ExceptionHandle>(1, Exception.Ptr()).Ptr();
}

SharedPtr<ValueHandle> ValueHandle::Null(ParserContext* Context)
{
	static SharedPtr<NullHandle> Value;
	if (!Value.IsValid()) {
		Value = NullHandle::Alloc<NullHandle>();
		Value->Context = Context;
		Value->Immutable = true;
	}
	return Value.Ptr();
}

SharedPtr<ValueHandle> ValueHandle::True(ParserContext* Context)
{
	static SharedPtr<IntHandle> Value;
	if (!Value.IsValid()) {
		Value = IntHandle::Alloc<IntHandle>();
		Value->Context = Context;
		Value->SetValue(1);
		Value->Immutable = true;
	}
	return Value.Ptr();
}

SharedPtr<ValueHandle> ValueHandle::False(ParserContext* Context)
{
	static SharedPtr<IntHandle> Value;
	if (!Value.IsValid()) {
		Value = IntHandle::Alloc<IntHandle>();
		Value->Context = Context;
		Value->SetValue(0);
		Value->Immutable = true;
	}
	return Value.Ptr();
}

CStrPtr ValueHandle::ToString()
{
	StringRep->Set("value_handle_null");
	return StringRep->CStr();
}
