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
    SetPosition(nullptr, nullptr);
    Error = nullptr;
    Context = nullptr;
}

void ValueHandle::SetPosition(SharedPtr<LexerPosition> pStartPosition, SharedPtr<LexerPosition> pEndPosition)
{
    StartPosition = pStartPosition;
    EndPosition = pEndPosition;
}

CStrPtr ValueHandle::ToString()
{
    memset(TmpNumber_Allocation, 0, sizeof(TmpNumber_Allocation));
    strcpy(TmpNumber_Allocation, "Null");
    return TmpNumber_Allocation;
}