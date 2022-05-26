//
// GameSencha, LLC 5/24/22.
//
#include "ParserNumberNode.hpp"
#include "Token.hpp"
SymplNamespace

ParserNumberNode::ParserNumberNode(Token* pNodeToken)
    : Type(ParseNodeType::Number)
{
    NodeToken = pNodeToken;
}

CStrPtr ParserNumberNode::ToString()
{
    memset(TmpNodeString_Allocate, 0, sizeof(TmpNodeString_Allocate));
    strcpy(TmpNodeString_Allocate, NodeToken->ToString());

    return TmpNodeString_Allocate;
}
