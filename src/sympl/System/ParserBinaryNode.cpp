//
// GameSencha, LLC 5/24/22.
//
#include "ParserBinaryNode.hpp"
#include "ParserNode.hpp"
#include "Token.hpp"
#include <fmt/format.h>
SymplNamespace

ParserBinaryNode::ParserBinaryNode(ParserNode* pLeftNode, class Token* pNodeToken, ParserNode* pRightNode)
    : ParserNode(pNodeToken)
{
    Type = ParseNodeType::Binary;
    LeftNode = pLeftNode;
    RightNode = pRightNode;
}

CStrPtr ParserBinaryNode::ToString()
{
    memset(TmpNodeString_Allocate, 0, strlen(TmpNodeString_Allocate));
    strcpy(TmpNodeString_Allocate, fmt::format(
            "{0}, {1}, {2}",
            LeftNode->ToString(),
            NodeToken->ToString(),
            RightNode->ToString()
    ).c_str());
}

ParserBinaryNode& ParserBinaryNode::operator=(const ParserBinaryNode& rhs)
{
    if (this == &rhs) {
        return *this;
    }

    LeftNode = rhs.LeftNode;
    NodeToken = rhs.NodeToken;
    RightNode = rhs.RightNode;

    return *this;
}
