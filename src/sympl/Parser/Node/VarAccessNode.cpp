//
// GameSencha, LLC 5/26/22.
//
#include "VarAccessNode.hpp"
#include "sympl/Parser/Token.hpp"
SymplNamespace

void VarAccessNode::__Construct(int argc, va_list ArgList)
{
    SharedPtr<Token> pToken = va_arg(ArgList, Token*);

    Create(pToken);
}

void VarAccessNode::Create(const SharedPtr<Token>& pNodeToken)
{
    ParserNode::Create(ParseNodeType::VarAccess, pNodeToken);

    StartPosition = NodeToken->GetStartPosition();
    EndPosition = NodeToken->GetEndPosition();
}

CStrPtr VarAccessNode::ToString()
{
    memset(TmpNodeString_Allocate, 0, sizeof(TmpNodeString_Allocate));
    strcpy(TmpNodeString_Allocate, fmt::format(
            "{0}",
            NodeToken->ToString()
    ).c_str());

    return TmpNodeString_Allocate;
}
