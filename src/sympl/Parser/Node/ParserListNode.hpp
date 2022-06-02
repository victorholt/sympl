//
// GameSencha, LLC 6/2/22.
//
#pragma once
#include "sympl/SymplPCH.hpp"
#include "ParserNode.hpp"

SymplNamespaceStart

class SYMPL_API ParserListNode : public ParserNode
{
    SYMPL_OBJECT(ParserListNode, ParserNode)

public:
    // List of arguments for the call.
    std::vector<SharedPtr<ParserNode>> ElementNodeList;

    /**
     * Overrides the construct.
     * @param argc
     * @param ArgList
     */
    virtual void __Construct(int argc, va_list ArgList) override;

    /**
     * Constructor
     * @param pNodeToken
     * @param pElementNodeList
     */
    void Create(
        const SharedPtr<class Token>& pNodeToken,
        const std::vector<SharedPtr<ParserNode>>& pElementNodeList,
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
