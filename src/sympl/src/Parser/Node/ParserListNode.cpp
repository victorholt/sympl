//
// GameSencha, LLC 6/2/22.
//
#include <sympl/include/Parser/Node/ParserListNode.hpp>
#include <sympl/include/Parser/Token.hpp>
SymplNamespace

void ParserListNode::__Construct(int argc, va_list ArgList)
{
}

void ParserListNode::__Destruct()
{
    ParserNode::__Destruct();
    ElementNodeList.clear();
}

void ParserListNode::Create(
    const SharedPtr<Token>& pNodeToken,
    const std::vector<SharedPtr<ParserNode>>& pElementNodeList,
    const SharedPtr<class LexerPosition>& pStartPosition,
    const SharedPtr<class LexerPosition>& pEndPosition
)
{
    ParserNode::Create(ParseNodeType::List, pNodeToken);
    ElementNodeList = pElementNodeList;

    StartPosition = pStartPosition;
    EndPosition = pEndPosition;
}

CStrPtr ParserListNode::ToString()
{
    memset(TmpNodeString_Allocate, 0, sizeof(TmpNodeString_Allocate));
    strcpy(TmpNodeString_Allocate, fmt::format(
            "({0})",
            NodeToken->ToString()
    ).c_str());

    return TmpNodeString_Allocate;
}
