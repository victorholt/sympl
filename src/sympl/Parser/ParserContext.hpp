//
// GameSencha, LLC 5/25/22.
//
#pragma once
#include "sympl/SymplPCH.hpp"
#include <sympl/Memory/ManagedObject.hpp>

SymplNamespaceStart

class ParserContext : public ManagedObject
{
public:
    // Reference to the parent context.
    SharedPtr<ParserContext> Parent;
    // Reference to the parent entry position.
    SharedPtr<class LexerPosition> ParentEntryPosition;
    // Reference to the display name.
    SharedPtr<class StringBuffer> DisplayName;
    // Reference to the global symbol table.
    SharedPtr<class SymbolTable> VariableSymbolTable;

    /**
     * Constructor.
     */
    ParserContext();

    /**
     * Creat
     * @param pParent
     * @param pParentEntryPosition
     * @param DisplayName
     */
    void Create(SharedPtr<ParserContext> pParent, SharedPtr<class LexerPosition> pParentEntryPosition, CStrPtr pDisplayName);
};

SymplNamespaceEnd
