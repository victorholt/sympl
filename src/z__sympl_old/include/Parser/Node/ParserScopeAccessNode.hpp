//
// GameSencha, LLC 6/3/22.
//
#pragma once
#include <__sympl_old/SymplPCH.hpp>
#include "ParserNode.hpp"

SymplNamespaceStart

class SYMPL_API ParserScopeAccessNode : public ParserNode
{
    SYMPL_OBJECT(ParserScopeAccessNode, ParserNode)

protected:
    // Parent scope token for accessing the variable.
    SharedPtr<class Token> ParentScopeToken;

    // Node if there is a variable assignment.
    SharedPtr<ParserNode> AssignNode;

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
     * @param pParentScopeToken
     */
    void Create(
        const SharedPtr<class Token>& pNodeToken,
        const SharedPtr<class Token>& pParentScopeToken
    );

    /**
     * Returns the parent scope node.
     * @return
     */
    SharedPtr<class Token> GetParentScopeToken() const;

    /**
     * Returns the assigned node.
     * @return
     */
    SharedPtr<ParserNode> GetAssignNode() const;

    /**
     * Sets the assignment node.
     * @param Value
     */
    void SetAssignNode(const SharedPtr<ParserNode>& Value);
};

SymplNamespaceEnd
