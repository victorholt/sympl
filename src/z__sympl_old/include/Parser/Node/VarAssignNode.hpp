//
// GameSencha, LLC 5/26/22.
//
#pragma once
#include <__sympl_old/SymplPCH.hpp>
#include "ParserNode.hpp"

SymplNamespaceStart

class SYMPL_API VarAssignNode : public ParserNode
{
	SYMPL_OBJECT(VarAssignNode, ParserNode)

public:
    // Expression result.
    SharedPtr<ParserNode> Value;

    /**
     * Overrides the construct.
     * @param argc
     * @param ArgList
     */
    void __Construct(int argc, va_list ArgList) override;

    /**
     * Destructor.
     */
    void __Destruct() override;

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
