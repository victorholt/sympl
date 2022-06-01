//
// Created by Victor on 5/26/2022.
//
#include "ValueHandle.hpp"
#include <sympl/Parser/ParserContext.hpp>
#include <sympl/Parser/LexerPosition.hpp>
#include <sympl/Parser/Error/RuntimeError.hpp>
SymplNamespace

ValueHandle::ValueHandle()
{
    Type = ValueType::Null;
    SetPosition(LexerPosition::Alloc<LexerPosition>(), LexerPosition::Alloc<LexerPosition>());
    Error = nullptr;
    Context = nullptr;
}

void ValueHandle::SetPosition(SharedPtr<LexerPosition> pStartPosition, SharedPtr<LexerPosition> pEndPosition)
{
    StartPosition = pStartPosition;
    EndPosition = pEndPosition;
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

SharedPtr<ValueHandle> ValueHandle::Copy() const
{
    return ValueHandle::BaseCopy<ValueHandle>(this);
}

CStrPtr ValueHandle::ToString()
{
    memset(TmpNumber_Allocation, 0, sizeof(TmpNumber_Allocation));
    strcpy(TmpNumber_Allocation, "Null");
    return TmpNumber_Allocation;
}
