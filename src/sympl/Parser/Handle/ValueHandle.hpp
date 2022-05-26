//
// GameSencha, LLC 5/26/22.
//
#pragma once
#include <sympl/SymplPCH.hpp>
#include <sympl/Memory/ManagedObject.hpp>

SymplNamespaceStart

class ValueHandle : public ManagedObject
{
public:
    ValueType Type = ValueType::Int;
    SharedPtr<class ParserError> Error;

protected:

    // String representation of the number.
    char TmpNumber_Allocation[64];

    // Start position of the number value.
    SharedPtr<struct LexerPosition> StartPosition;
    // End position of the number value.
    SharedPtr<struct LexerPosition> EndPosition;

public:
    /**
     * Constructor.
     */
    ValueHandle();

    /**
     * Sets the position of the number handle.
     * @param pStartPosition
     * @param pEndPosition
     */
    void SetPosition(SharedPtr<class LexerPosition> pStartPosition, SharedPtr<class LexerPosition> pEndPosition);

    /**
     * Returns a string of the value.
     * @return
     */
    virtual CStrPtr ToString();
};

SymplNamespaceEnd
