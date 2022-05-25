//
// GameSencha, LLC 5/24/22.
//
#pragma once
#include "SymplPCH.hpp"
#include "ParserNode.hpp"

SymplNamespaceStart

class ParserBinaryNode : public ParserNode
{
public:
    // Reference to the left node.
    class ParserNode* LeftNode;
    // Reference to the right node.
    class ParserNode* RightNode;

    /**
     * Constructor
     * @param pLeftNode
     * @param pNodeToken
     * @param pRightNode
     */
    ParserBinaryNode(class ParserNode* pLeftNode, class Token* pNodeToken, class ParserNode* pRightNode);

    /**
     * Returns the string representation of the node.
     * @return
     */
    virtual CStrPtr ToString() override;

    /**
     * Copies the object from another object.
     * @param rhs
     * @return
     */
    ParserBinaryNode& operator=(const ParserBinaryNode& rhs);
};

SymplNamespaceEnd
