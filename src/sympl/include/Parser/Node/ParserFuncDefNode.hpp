//
// GameSencha, LLC 6/1/22.
//
#pragma once
#include <sympl/SymplPCH.hpp>
#include "ParserNode.hpp"

SymplNamespaceStart

class SYMPL_API ParserFuncDefNode : public ParserNode
{
    SYMPL_OBJECT(ParserFuncDefNode, ParserNode)

public:
    // List of argument name tokens.
    std::vector<SharedPtr<class Token>> ArgNameTokenList;

    // Body node to evaluate after the function has been called.
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
     * @param pArgNameTokenList
     * @param pBody
     */
    void Create(
        const SharedPtr<class Token>& pNodeToken,
        const std::vector<SharedPtr<class Token>>& pArgNameTokenList,
        const SharedPtr<ParserNode>& pBody
    );

    /**
     * Returns the string representation of the node.
     * @return
     */
    virtual CStrPtr ToString() override;
};

SymplNamespaceEnd
