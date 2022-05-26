//
// GameSencha, LLC 5/25/22.
//
#include "ParserUnaryOpNode.hpp"
#include "sympl/Parser/Token.hpp"
SymplNamespace

ParserUnaryOpNode::ParserUnaryOpNode(Token* pNodeToken, SharedPtr<ParserNode> pNode)
        : ParserNode(ParseNodeType::Unary, pNodeToken)
{
    Node = pNode;
    StartPosition = NodeToken->GetStartPosition();
    EndPosition = Node->EndPosition;
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
