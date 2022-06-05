//
// GameSencha, LLC 6/3/22.
//
#pragma once
#include <sympl/SymplPCH.hpp>
#include "ParserNode.hpp"

SymplNamespaceStart

class SYMPL_API ParserReturnNode : public ParserNode
{
    SYMPL_OBJECT(ParserReturnNode, ParserNode)

public:
    // Type of node.
    SharedPtr<ParserNode> NodeToReturn;

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
        const SharedPtr<ParserNode>& pNodeToReturn,
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
