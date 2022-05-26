//
// GameSencha, LLC 5/24/22.
//
#pragma once
#include "sympl/SymplPCH.hpp"
#include "ParserNode.hpp"
#include "sympl/Memory/SharedPtr.hpp"

SymplNamespaceStart

class ParserBinaryOpNode : public ParserNode
{
public:
    // Reference to the left node.
    SharedPtr<ParserNode> LeftNode;
    // Reference to the right node.
    SharedPtr<ParserNode> RightNode;

    /**
     * Constructor
     * @param pLeftNode
     * @param pNodeToken
     * @param pRightNode
     */
    ParserBinaryOpNode(const SharedPtr<ParserNode>& pLeftNode, class Token* pNodeToken, const SharedPtr<ParserNode>& pRightNode);

    /**
     * Returns the string representation of the node.
     * @return
     */
    virtual CStrPtr ToString() override;
};

SymplNamespaceEnd
