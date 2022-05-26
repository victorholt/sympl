//
// GameSencha, LLC 5/25/22.
//
#include "ParserNode.hpp"
#include "sympl/Parser/Token.hpp"
SymplNamespace

ParserNode::ParserNode(ParseNodeType pType, Token* pNodeToken)
        : Type(pType)
{
    NodeToken = pNodeToken;
    StartPosition = NodeToken->GetStartPosition();
    EndPosition = NodeToken->GetEndPosition();
}

CStrPtr ParserNode::ToString()
{
    memset(TmpNodeString_Allocate, 0, sizeof(TmpNodeString_Allocate));
    strcpy(TmpNodeString_Allocate, NodeToken->ToString());

    return TmpNodeString_Allocate;
}
