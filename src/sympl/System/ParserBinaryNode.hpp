//
// GameSencha, LLC 5/24/22.
//
#pragma once
#include "SymplPCH.hpp"
#include "ParserNumberNode.hpp"
#include <sympl/Memory/SharedPtr.hpp>

SymplNamespaceStart

class ParserBinaryNode : public ParserNumberNode
{
public:
    // Reference to the left node.
    SharedPtr<ParserNumberNode> LeftNode;
    // Reference to the right node.
    SharedPtr<ParserNumberNode> RightNode;

    /**
     * Constructor
     * @param pLeftNode
     * @param pNodeToken
     * @param pRightNode
     */
    ParserBinaryNode(const SharedPtr<ParserNumberNode>& pLeftNode, class Token* pNodeToken, const SharedPtr<ParserNumberNode>& pRightNode);

    /**
     * Returns the string representation of the node.
     * @return
     */
    virtual CStrPtr ToString() override;
};

SymplNamespaceEnd
