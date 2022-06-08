//
// GameSencha, LLC 6/1/22.
//
#include <sympl/include/Parser/Node/ParserWhileNode.hpp>
#include <sympl/include/Parser/Token.hpp>
SymplNamespace

void ParserWhileNode::__Construct(int argc, va_list ArgList)
{
	SharedPtr<Token> pToken = va_arg(ArgList, Token*);
	SharedPtr<ParserNode> pCondition = va_arg(ArgList, ParserNode*);
	SharedPtr<ParserNode> pBody = va_arg(ArgList, ParserNode*);

	Create(pToken, pCondition, pBody);
}

void ParserWhileNode::Create(
	const SharedPtr<class Token>& pNodeToken,
	const SharedPtr<ParserNode>& pCondition,
	const SharedPtr<ParserNode>& pBody
)
{
	ParserNode::Create(ParseNodeType::While, pNodeToken);

	ConditionNode = pCondition;
	BodyNode = pBody;

	StartPosition = ConditionNode->GetStartPosition();
	EndPosition = BodyNode->GetEndPosition();
}

CStrPtr ParserWhileNode::ToString()
{
	memset(TmpNodeString_Allocate, 0, sizeof(TmpNodeString_Allocate));
	strcpy(TmpNodeString_Allocate, fmt::format(
			"({0})",
			NodeToken->ToString()
	).c_str());

	return TmpNodeString_Allocate;
}
