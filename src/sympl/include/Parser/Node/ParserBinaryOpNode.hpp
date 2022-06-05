//
// GameSencha, LLC 5/24/22.
//
#pragma once
#include <sympl/SymplPCH.hpp>
#include "ParserNode.hpp"
#include "sympl/include/Memory/SharedPtr.hpp"

SymplNamespaceStart

class SYMPL_API ParserBinaryOpNode : public ParserNode
{
	SYMPL_OBJECT(ParserBinaryOpNode, ParserNode)

public:
    // Reference to the left node.
    SharedPtr<ParserNode> LeftNode;
    // Reference to the right node.
    SharedPtr<ParserNode> RightNode;

    /**
     * Overrides the construct.
     * @param argc
     * @param ArgList
     */
    virtual void __Construct(int argc, va_list ArgList) override;

    /**
     * Constructor
     * @param pLeftNode
     * @param pNodeToken
     * @param pRightNode
     */
    void Create(const SharedPtr<ParserNode>& pLeftNode, SharedPtr<class Token> pNodeToken, const SharedPtr<ParserNode>& pRightNode);

    /**
     * Returns the string representation of the node.
     * @return
     */
    virtual CStrPtr ToString() override;
};

SymplNamespaceEnd
