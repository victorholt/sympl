//
// GameSencha, LLC 6/1/22.
//
#include <sympl/include/Parser/Node/ParserIfNode.hpp>
#include <sympl/include/Parser/Token.hpp>
SymplNamespace

void ParserIfNode::__Construct(int argc, va_list ArgList)
{
}

void ParserIfNode::Create(const SharedPtr<class Token>& pNodeToken, const IfCaseList& pCases, const ElseCaseTuple& pElseCase)
{
	ParserNode::Create(ParseNodeType::If, pNodeToken);

	Cases = pCases;
	ElseCase = pElseCase;

	StartPosition = NodeToken->GetStartPosition();

	// Grab the start position from the cases.
	if (!Cases.empty()) {
		StartPosition = std::get<0>(Cases[0])->GetStartPosition();
	}

	// Grab the end position.
    auto ElseCaseNode = std::get<0>(pElseCase);
	if (ElseCaseNode.IsValid()) {
		EndPosition = ElseCaseNode->GetEndPosition();
	} else if (!Cases.empty()) {
		EndPosition = std::get<0>(Cases[Cases.size() - 1])->GetEndPosition();
	}
}

CStrPtr ParserIfNode::ToString()
{
    auto ElseCaseNode = std::get<0>(ElseCase);
    memset(TmpNodeString_Allocate, 0, sizeof(TmpNodeString_Allocate));
	strcpy(TmpNodeString_Allocate, fmt::format(
			"({0}, {1})",
			NodeToken->ToString(),
            ElseCaseNode->ToString()
	).c_str());

	return TmpNodeString_Allocate;
}
