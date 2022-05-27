//
// GameSencha, LLC 5/26/22.
//
#pragma once
#include "sympl/SymplPCH.hpp"
#include "ParserNode.hpp"

SymplNamespaceStart

class VarAssignNode : public ParserNode
{
public:
    // Expression result.
    SharedPtr<ParserNode> Value;

    /**
     * Overrides the construct.
     * @param argc
     * @param ArgList
     */
    virtual void __Construct(int argc, va_list ArgList) override;

    /**
     * Constructor
     * @param pNodeToken
     * @param pValue
     */
    void Create(const SharedPtr<class Token>& pNodeToken, const SharedPtr<ParserNode>& pValue);

    /**
     * Returns the string representation of the node.
     * @return
     */
    virtual CStrPtr ToString() override;
};

SymplNamespaceEnd
