//
// GameSencha, LLC 6/3/22.
//
#pragma once
#include <sympl/SymplPCH.hpp>
#include "ParserNode.hpp"

SymplNamespaceStart

class SYMPL_API ParserScopeAccessNode : public ParserNode
{
    SYMPL_OBJECT(ParserScopeAccessNode, ParserNode)

public:
    // Parent scope token for accessing the variable.
    SharedPtr<class Token> ParentScopeToken;
    // Node if there is a variable assignment.
    SharedPtr<ParserNode> AssignNode;

    /**
     * Overrides the construct.
     * @param argc
     * @param ArgList
     */
    virtual void __Construct(int argc, va_list ArgList) override;

    /**
     * Constructor
     * @param pNodeToken
     * @param pParentScopeToken
     */
    void Create(
        const SharedPtr<class Token>& pNodeToken,
        const SharedPtr<class Token>& pParentScopeToken
    );
};

SymplNamespaceEnd
