//
// GameSencha, LLC 6/3/22.
//
#include "ParserBreakNode.hpp"
#include "sympl/Parser/Token.hpp"
#include <sympl/Parser/LexerPosition.hpp>
SymplNamespace

void ParserBreakNode::__Construct(int argc, va_list ArgList)
{
    SharedPtr<Token> pToken = va_arg(ArgList, Token*);
    SharedPtr<LexerPosition> pStartPosition = va_arg(ArgList, LexerPosition*);
    SharedPtr<LexerPosition> pEndPosition = va_arg(ArgList, LexerPosition*);

    Create(pToken, pStartPosition, pEndPosition);
}

void ParserBreakNode::Create(
    const SharedPtr<class Token>& pNodeToken,
    const SharedPtr<class LexerPosition>& pStartPosition,
    const SharedPtr<class LexerPosition>& pEndPosition
)
{
    ParserNode::Create(ParseNodeType::Break, pNodeToken);

    StartPosition = pStartPosition;
    EndPosition = pEndPosition;
}

CStrPtr ParserBreakNode::ToString()
{
    memset(TmpNodeString_Allocate, 0, sizeof(TmpNodeString_Allocate));
    strcpy(TmpNodeString_Allocate, fmt::format(
            "({0})",
            NodeToken->ToString()
    ).c_str());

    return TmpNodeString_Allocate;
}
