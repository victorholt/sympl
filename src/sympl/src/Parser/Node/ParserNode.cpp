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
    NodeToken = pNodeToken->Copy();

    if (NodeToken.IsValid()) {
        StartPosition = NodeToken->GetStartPosition();
        EndPosition = NodeToken->GetEndPosition();
    }
}

SharedPtr<Token> ParserNode::GetNodeToken() const
{
    return NodeToken->Copy();
}

void ParserNode::SetNodeToken(const SharedPtr<Token>& pNodeToken)
{
    NodeToken = pNodeToken;
}

SharedPtr<LexerPosition> ParserNode::GetStartPosition() const
{
    return StartPosition->Copy();
}

SharedPtr<LexerPosition> ParserNode::GetEndPosition() const
{
    return EndPosition->Copy();
}

void
ParserNode::SetPosition(const SharedPtr<LexerPosition>& pStartPosition,
                        const SharedPtr<LexerPosition>& pEndPosition)
{
    StartPosition = pStartPosition;
    EndPosition = pEndPosition;
}

CStrPtr ParserNode::ToString()
{
    memset(TmpNodeString_Allocate, 0, sizeof(TmpNodeString_Allocate));
    strcpy(TmpNodeString_Allocate, NodeToken->ToString());

    return TmpNodeString_Allocate;
}
