//
// GameSencha, LLC 6/3/22.
//
#pragma once
#include <__sympl_old/SymplPCH.hpp>
#include "ParserNode.hpp"

SymplNamespaceStart

class SYMPL_API ParserObjectNode : public ParserNode
{
    SYMPL_OBJECT(ParserObjectNode, ParserNode)

public:
    // Body node for the parser.
    SharedPtr<ParserNode> BodyNode;

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
    void Create(
        const SharedPtr<class Token>& pNodeToken,
        const SharedPtr<ParserNode>& pBodyNode
    );
};

SymplNamespaceEnd
