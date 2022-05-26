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
     * Overrides the construct.
     * @param argc
     * @param ArgList
     */
    virtual void __Construct(int argc, va_list ArgList) override;

    /**
     * Constructor
     * @param pNodeToken
     */
    void Create(SharedPtr<class Token> pNodeToken, SharedPtr<ParserNode> pNode);

    /**
     * Returns the string representation of the node.
     * @return
     */
    virtual CStrPtr ToString() override;
};

SymplNamespaceEnd
