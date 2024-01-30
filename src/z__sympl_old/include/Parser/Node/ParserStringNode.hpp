//
// GameSencha, LLC 6/2/22.
//
#pragma once
#include <__sympl_old/SymplPCH.hpp>
#include "ParserNode.hpp"

SymplNamespaceStart

class SYMPL_API ParserStringNode : public ParserNode
{
    SYMPL_OBJECT(ParserStringNode, ParserNode)

public:
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
     */
    void Create(const SharedPtr<class Token>& pNodeToken);
};

SymplNamespaceEnd
