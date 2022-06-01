//
// GameSencha, LLC 5/24/22.
//
#pragma once
#include "sympl/SymplPCH.hpp"
#include "sympl/Memory/ManagedObject.hpp"

SymplNamespaceStart

class SYMPL_API ParserNode : public ManagedObject
{
	SYMPL_OBJECT(ParserNode, ManagedObject)

protected:
    // Handles conversion of the node to a string.
    char TmpNodeString_Allocate[255];

public:
    // Type of node.
    ParseNodeType Type;

    // Reference to the token associated with the node.
    SharedPtr<class Token> NodeToken;

    // Start position of the token.
    SharedPtr<class LexerPosition> StartPosition;

    // End position of the token.
    SharedPtr<class LexerPosition> EndPosition;

    /**
     * Overrides the construct.
     * @param argc
     * @param ArgList
     */
    virtual void __Construct(int argc, va_list ArgList) override;

    /**
     * Creates the parser node.
     * @param pType
     * @param pNodeToken
     */
    void Create(ParseNodeType pType, SharedPtr<class Token> pNodeToken);

    /**
     * Returns the string representation of the node.
     * @return
     */
    virtual CStrPtr ToString();
};

SymplNamespaceEnd
