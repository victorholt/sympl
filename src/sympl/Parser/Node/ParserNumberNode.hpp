//
// GameSencha, LLC 5/24/22.
//
#pragma once
#include "sympl/SymplPCH.hpp"
#include "ParserNode.hpp"

SymplNamespaceStart

class ParserNumberNode : public ParserNode
{
public:
    /**
     * Constructor
     * @param pNodeToken
     */
    ParserNumberNode(class Token* pNodeToken);
};

SymplNamespaceEnd
