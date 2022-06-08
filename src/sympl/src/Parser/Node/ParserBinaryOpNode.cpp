//
// GameSencha, LLC 5/24/22.
//
#include <sympl/include/Parser/Node/ParserBinaryOpNode.hpp>
#include <sympl/include/Parser/Token.hpp>
#include <sympl/thirdparty/fmt/format.h>
SymplNamespace

void ParserBinaryOpNode::__Construct(int argc, va_list ArgList)
{
    SharedPtr<ParserNode> pLeftNode = va_arg(ArgList, ParserNode*);
    SharedPtr<Token> pToken = va_arg(ArgList, Token*);
    SharedPtr<ParserNode> pRightNode = va_arg(ArgList, ParserNode*);

    Create(pLeftNode, pToken, pRightNode);
}

void ParserBinaryOpNode::Create(const SharedPtr<ParserNode>& pLeftNode, SharedPtr<Token> pNodeToken, const SharedPtr<ParserNode>& pRightNode)
{
    ParserNode::Create(ParseNodeType::Binary, pNodeToken);

    LeftNode = pLeftNode;
    RightNode = pRightNode;

    StartPosition = LeftNode->GetStartPosition();
    EndPosition = RightNode->GetEndPosition();
}

SharedPtr<ParserNode> ParserBinaryOpNode::GetLeftNode() const
{
    return LeftNode;
}

SharedPtr<ParserNode> ParserBinaryOpNode::GetRightNode() const
{
    return RightNode;
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
