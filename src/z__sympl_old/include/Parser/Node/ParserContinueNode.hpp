//
// GameSencha, LLC 6/3/22.
//
#pragma once
#include <__sympl_old/SymplPCH.hpp>
#include "ParserNode.hpp"

SymplNamespaceStart

class SYMPL_API ParserContinueNode : public ParserNode
{
    SYMPL_OBJECT(ParserContinueNode, ParserNode)

public:
    /**
     * Overrides the construct.
     * @param argc
     * @param ArgList
     */
    virtual void __Construct(int argc, va_list ArgList) override;

    /**
     * Constructor
     * @param pNodeToken
     * @param pNodeToReturn
     */
    void Create(
        const SharedPtr<class Token>& pNodeToken,
        const SharedPtr<class LexerPosition>& pStartPosition,
        const SharedPtr<class LexerPosition>& pEndPosition
    );

    /**
     * Returns the string representation of the node.
     * @return
     */
    virtual CStrPtr ToString() override;
};

SymplNamespaceEnd
