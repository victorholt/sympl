//
// GameSencha, LLC 5/25/22.
//
#include <sympl/include/Parser/Node/ParserNode.hpp>
#include <sympl/include/Parser/Token.hpp>
#include <sympl/include/Parser/LexerPosition.hpp>
SymplNamespace

void ParserNode::__Construct(int argc, va_list ArgList)
{
    ParseNodeType pType = va_arg(ArgList, ParseNodeType);
    SharedPtr<Token> pNodeToken = va_arg(ArgList, Token*);

    Create(pType, pNodeToken);
}

void ParserNode::__Destruct()
{
    NodeToken = nullptr;
    StartPosition = nullptr;
    EndPosition = nullptr;
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
