//
// GameSencha, LLC 5/25/22.
//
#pragma once
#include "sympl/SymplPCH.hpp"
#include "ParserNumberNode.hpp"

SymplNamespaceStart

class ParserUnaryOpNode : public ParserNode
{
public:
    // Type of node.
    SharedPtr<ParserNode> Node;

    /**
     * Constructor
     * @param pNodeToken
     */
    ParserUnaryOpNode(class Token* pNodeToken, SharedPtr<ParserNode> pNode);

    /**
     * Returns the string representation of the node.
     * @return
     */
    virtual CStrPtr ToString() override;
};

SymplNamespaceEnd
