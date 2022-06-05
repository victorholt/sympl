//
// GameSencha, LLC 5/26/22.
//
#pragma once
#include <sympl/SymplPCH.hpp>
#include "ParserNode.hpp"

SymplNamespaceStart

class SYMPL_API VarAccessNode : public ParserNode
{
	SYMPL_OBJECT(VarAccessNode, ParserNode)

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
     */
    void Create(const SharedPtr<class Token>& pNodeToken);

    /**
     * Returns the string representation of the node.
     * @return
     */
    virtual CStrPtr ToString() override;
};

SymplNamespaceEnd
