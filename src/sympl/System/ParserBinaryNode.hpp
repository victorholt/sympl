//
// GameSencha, LLC 5/24/22.
//
#pragma once
#include "SymplPCH.hpp"
#include "ParserNumberNode.hpp"

SymplNamespaceStart

class ParserBinaryNode : public ParserNumberNode
{
public:
    // Reference to the left node.
    ParserNumberNode* LeftNode;
    // Reference to the right node.
    ParserNumberNode* RightNode;

    /**
     * Constructor
     * @param pLeftNode
     * @param pNodeToken
     * @param pRightNode
     */
    ParserBinaryNode(ParserNumberNode* pLeftNode, class Token* pNodeToken, ParserNumberNode* pRightNode);

    /**
     * Returns the string representation of the node.
     * @return
     */
    virtual CStrPtr ToString() override;
};

SymplNamespaceEnd
