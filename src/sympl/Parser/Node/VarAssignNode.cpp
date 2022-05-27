//
// GameSencha, LLC 5/26/22.
//
#include "VarAssignNode.hpp"
#include "sympl/Parser/Token.hpp"
#include "sympl/Parser/ParseResult.hpp"
SymplNamespace

void VarAssignNode::__Construct(int argc, va_list ArgList)
{
    SharedPtr<Token> pToken = va_arg(ArgList, Token*);
    SharedPtr<ParserNode> pValue = va_arg(ArgList, ParserNode*);

    Create(pToken, pValue);
}

void VarAssignNode::Create(const SharedPtr<Token>& pNodeToken, const SharedPtr<ParserNode>& pValue)
{
    ParserNode::Create(ParseNodeType::VarAssign, pNodeToken);

    Value = pValue;
    StartPosition = NodeToken->GetStartPosition();
    EndPosition = NodeToken->GetEndPosition();
}

CStrPtr VarAssignNode::ToString()
{
    memset(TmpNodeString_Allocate, 0, sizeof(TmpNodeString_Allocate));
    strcpy(TmpNodeString_Allocate, fmt::format(
            "{0}",
            NodeToken->ToString()
    ).c_str());

    return TmpNodeString_Allocate;
}
