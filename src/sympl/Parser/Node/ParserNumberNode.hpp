//
// GameSencha, LLC 5/24/22.
//
#pragma once
#include "sympl/SymplPCH.hpp"
#include "ParserNode.hpp"

SymplNamespaceStart

class SYMPL_API ParserNumberNode : public ParserNode
{
	SYMPL_OBJECT(ParserNumberNode, ParserNode)

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
};

SymplNamespaceEnd
