//
// GameSencha, LLC 5/24/22.
//
#include "ParserBinaryOpNode.hpp"
#include "sympl/Parser/Token.hpp"
#include "sympl/thirdparty/fmt/format.h"
SymplNamespace

ParserBinaryOpNode::ParserBinaryOpNode(const SharedPtr<ParserNode>& pLeftNode, class Token* pNodeToken, const SharedPtr<ParserNode>& pRightNode)
    : ParserNode(ParseNodeType::Binary, pNodeToken)
{
    LeftNode = pLeftNode;
    RightNode = pRightNode;

    StartPosition = LeftNode->StartPosition;
    EndPosition = RightNode->EndPosition;
}

CStrPtr ParserBinaryOpNode::ToString()
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
