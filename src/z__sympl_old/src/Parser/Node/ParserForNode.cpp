//
// GameSencha, LLC 6/1/22.
//
#include <__sympl_old/include/Parser/Node/ParserForNode.hpp>
#include <__sympl_old/include/Parser/Token.hpp>
SymplNamespace

void ParserForNode::__Construct(int argc, va_list ArgList)
{
	SharedPtr<Token> pToken = va_arg(ArgList, Token*);
	SharedPtr<ParserNode> pStartValue = va_arg(ArgList, ParserNode*);
	SharedPtr<ParserNode> pEndValue = va_arg(ArgList, ParserNode*);
	SharedPtr<ParserNode> pStepValue = va_arg(ArgList, ParserNode*);
	SharedPtr<ParserNode> pBody = va_arg(ArgList, ParserNode*);

	Create(pToken, pStartValue, pEndValue, pStepValue, pBody);
}

void ParserForNode::Create(
	const SharedPtr<class Token>& pNodeToken,
	const SharedPtr<ParserNode>& pStartValue,
	const SharedPtr<ParserNode>& pEndValue,
	const SharedPtr<ParserNode>& pStepValue,
	const SharedPtr<ParserNode>& pBody
)
{
	ParserNode::Create(ParseNodeType::For, pNodeToken);

	StartValueNode = pStartValue;
	EndValueNode = pEndValue;
	StepValueNode = pStepValue;
	BodyNode = pBody;

	StartPosition = NodeToken->GetStartPosition();
	EndPosition = BodyNode->GetEndPosition();
}

CStrPtr ParserForNode::ToString()
{
	memset(TmpNodeString_Allocate, 0, sizeof(TmpNodeString_Allocate));
	strcpy(TmpNodeString_Allocate, fmt::format(
			"({0})",
			NodeToken->ToString()
	).c_str());

	return TmpNodeString_Allocate;
}
