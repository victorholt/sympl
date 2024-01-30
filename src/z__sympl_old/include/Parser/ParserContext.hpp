//
// GameSencha, LLC 5/25/22.
//
#pragma once
#include <__sympl_old/SymplPCH.hpp>
#include <__sympl_old/include/Memory/ManagedObject.hpp>

SymplNamespaceStart

class SYMPL_API ParserContext : public ManagedObject
{
	SYMPL_OBJECT(ParserContext, ManagedObject)

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
    void __Construct(int argc, va_list Args);

    /**
     * Destructor.
     */
    void __Destruct() override;

    /**
     * Creat
     * @param pParent
     * @param pParentEntryPosition
     * @param DisplayName
     */
    void Create(SharedPtr<ParserContext> pParent, SharedPtr<class LexerPosition> pParentEntryPosition, CStrPtr pDisplayName);
};

SymplNamespaceEnd
