//
// GameSencha, LLC 5/24/22.
//
#pragma once
#include "sympl/SymplPCH.hpp"
#include "sympl/Memory/ManagedObject.hpp"

SymplNamespaceStart

class ParserNode : public ManagedObject
{
protected:
    // Handles conversion of the node to a string.
    char TmpNodeString_Allocate[255];

public:
    // Type of node.
    ParseNodeType Type;

    // Reference to the token associated with the node.
    class Token* NodeToken;

    // Start position of the token.
    SharedPtr<class LexerPosition> StartPosition;

    // End position of the token.
    SharedPtr<class LexerPosition> EndPosition;

    /**
     * Constructor
     * @param pType
     * @param pNodeToken
     */
    ParserNode(ParseNodeType pType, class Token* pNodeToken);

    /**
     * Returns the string representation of the node.
     * @return
     */
    virtual CStrPtr ToString();
};

SymplNamespaceEnd
