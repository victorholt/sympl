//
// GameSencha, LLC 6/3/22.
//
#include <__sympl_old/include/Parser/Node/ParserReturnNode.hpp>
#include <__sympl_old/include/Parser/Token.hpp>
#include <__sympl_old/include/Parser/LexerPosition.hpp>
SymplNamespace

void ParserReturnNode::__Construct(int argc, va_list ArgList)
{
    SharedPtr<Token> pToken = va_arg(ArgList, Token*);
    SharedPtr<ParserNode> pNodeToReturn = va_arg(ArgList, ParserNode*);
    SharedPtr<LexerPosition> pStartPosition = va_arg(ArgList, LexerPosition*);
    SharedPtr<LexerPosition> pEndPosition = va_arg(ArgList, LexerPosition*);

    Create(pToken, pNodeToReturn, pStartPosition, pEndPosition);
}

void ParserReturnNode::__Destruct()
{
    ParserNode::__Destruct();
    NodeToReturn = nullptr;
}

void ParserReturnNode::Create(
    const SharedPtr<class Token>& pNodeToken,
    const SharedPtr<ParserNode>& pNodeToReturn,
    const SharedPtr<class LexerPosition>& pStartPosition,
    const SharedPtr<class LexerPosition>& pEndPosition
)
{
    ParserNode::Create(ParseNodeType::Return, pNodeToken);

    NodeToReturn = pNodeToReturn;
    StartPosition = pStartPosition;
    EndPosition = pEndPosition;
}

CStrPtr ParserReturnNode::ToString()
{
    memset(TmpNodeString_Allocate, 0, sizeof(TmpNodeString_Allocate));
    strcpy(TmpNodeString_Allocate, fmt::format(
        "({0}, {1})",
        NodeToken->ToString(),
        NodeToReturn->ToString()
    ).c_str());

    return TmpNodeString_Allocate;
}
