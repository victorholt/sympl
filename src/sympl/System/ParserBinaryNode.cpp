//
// GameSencha, LLC 5/24/22.
//
#include "ParserBinaryNode.hpp"
#include "ParserNumberNode.hpp"
#include "Token.hpp"
#include <fmt/format.h>
SymplNamespace

ParserBinaryNode::ParserBinaryNode(const SharedPtr<ParserNumberNode>& pLeftNode, class Token* pNodeToken, const SharedPtr<ParserNumberNode>& pRightNode)
    : ParserNumberNode(pNodeToken)
{
    Type = ParseNodeType::Binary;
    LeftNode = pLeftNode;
    RightNode = pRightNode;
}

CStrPtr ParserBinaryNode::ToString()
{
    memset(TmpNodeString_Allocate, 0, sizeof(TmpNodeString_Allocate));
    strcpy(TmpNodeString_Allocate, fmt::format(
            "({0}, {1}, {2})",
            LeftNode->ToString(),
            NodeToken->ToString(),
            RightNode->ToString()
    ).c_str());

	return TmpNodeString_Allocate;
}
