//
// GameSencha, LLC 5/25/22.
//
#include "ParserNode.hpp"
#include "sympl/Parser/Token.hpp"
SymplNamespace

void ParserNode::__Construct(int argc, va_list ArgList)
{
    ParseNodeType pType = va_arg(ArgList, ParseNodeType);
    SharedPtr<Token> pNodeToken = va_arg(ArgList, Token*);

    Create(pType, pNodeToken);
}

void ParserNode::Create(ParseNodeType pType, const SharedPtr<Token>& pNodeToken)
{
    Type = pType;
    NodeToken = pNodeToken;

    if (NodeToken.IsValid()) {
        StartPosition = NodeToken->GetStartPosition();
        EndPosition = NodeToken->GetEndPosition();
    }
}

CStrPtr ParserNode::ToString()
{
    memset(TmpNodeString_Allocate, 0, sizeof(TmpNodeString_Allocate));
    strcpy(TmpNodeString_Allocate, NodeToken->ToString());

    return TmpNodeString_Allocate;
}
