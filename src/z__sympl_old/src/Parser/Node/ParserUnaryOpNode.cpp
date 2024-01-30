//
// GameSencha, LLC 5/25/22.
//
#include <__sympl_old/include/Parser/Node/ParserUnaryOpNode.hpp>
#include <__sympl_old/include/Parser/Token.hpp>
SymplNamespace

void ParserUnaryOpNode::__Construct(int argc, va_list ArgList)
{
    SharedPtr<Token> pToken = va_arg(ArgList, Token*);
    SharedPtr<ParserNode> pNode = va_arg(ArgList, ParserNode*);

    Create(pToken, pNode);
}

void ParserUnaryOpNode::Create(const SharedPtr<Token>& pNodeToken, const SharedPtr<ParserNode>& pNode)
{
    ParserNode::Create(ParseNodeType::Unary, pNodeToken);

    Node = pNode;
    StartPosition = NodeToken->GetStartPosition();
    EndPosition = Node->GetEndPosition();
}

CStrPtr ParserUnaryOpNode::ToString()
{
    memset(TmpNodeString_Allocate, 0, sizeof(TmpNodeString_Allocate));
    strcpy(TmpNodeString_Allocate, fmt::format(
            "({0}, {1})",
            NodeToken->ToString(),
            Node->ToString()
    ).c_str());

    return TmpNodeString_Allocate;
}
