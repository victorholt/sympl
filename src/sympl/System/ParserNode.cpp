//
// GameSencha, LLC 5/24/22.
//
#include "ParserNode.hpp"
#include "Token.hpp"
SymplNamespace

ParserNode::ParserNode(Token* pNodeToken)
    : Type(ParseNodeType::Number)
{
    NodeToken = pNodeToken;
}

CStrPtr ParserNode::ToString()
{
    memset(TmpNodeString_Allocate, 0, strlen(TmpNodeString_Allocate));
    strcpy(TmpNodeString_Allocate, NodeToken->ToString());

    return TmpNodeString_Allocate;
}

ParserNode& ParserNode::operator=(const ParserNode& rhs)
{
    if (this == &rhs) {
        return *this;
    }

    NodeToken = rhs.NodeToken;

    return *this;
}
