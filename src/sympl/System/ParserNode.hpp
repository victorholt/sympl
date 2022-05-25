//
// GameSencha, LLC 5/24/22.
//
#pragma once
#include "SymplPCH.hpp"

SymplNamespaceStart

class ParserNode
{
protected:
    // Handles conversion of the node to a string.
    char TmpNodeString_Allocate[255];

public:
    // Type of node.
    ParseNodeType Type;

    // Reference to the token associated with the node.
    class Token* NodeToken;

    /**
     * Constructor
     * @param pNodeToken
     */
    ParserNode(class Token* pNodeToken);

    /**
     * Returns the string representation of the node.
     * @return
     */
    virtual CStrPtr ToString();

    /**
     * Copies the parser node.
     * @param rhs
     * @return
     */
    ParserNode& operator=(const ParserNode& rhs);
};

SymplNamespaceEnd
