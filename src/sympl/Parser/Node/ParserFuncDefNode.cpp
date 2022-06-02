//
// GameSencha, LLC 6/1/22.
//
#include "ParserFuncDefNode.hpp"
#include "sympl/Parser/Token.hpp"
SymplNamespace

void ParserFuncDefNode::__Construct(int argc, va_list ArgList)
{
}

void ParserFuncDefNode::Create(
    const SharedPtr<Token>& pNodeToken,
    const std::vector<SharedPtr<Token>>& pArgNameTokenList,
    const SharedPtr<ParserNode>& pBody
)
{
    ParserNode::Create(ParseNodeType::Func, pNodeToken);

    ArgNameTokenList = pArgNameTokenList;
    BodyNode = pBody;

    if ( pNodeToken.IsValid() ) {
        StartPosition = pNodeToken->GetStartPosition();
    }
    else if (!ArgNameTokenList.empty()) {
        StartPosition = ArgNameTokenList[0]->GetStartPosition();
    }
    else {
        StartPosition = BodyNode->StartPosition;
    }

    EndPosition = BodyNode->EndPosition;
}

CStrPtr ParserFuncDefNode::ToString()
{
    memset(TmpNodeString_Allocate, 0, sizeof(TmpNodeString_Allocate));
    strcpy(TmpNodeString_Allocate, fmt::format(
            "({0})",
            NodeToken->ToString()
    ).c_str());

    return TmpNodeString_Allocate;
}
