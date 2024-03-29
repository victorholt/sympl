//
// GameSencha, LLC 5/26/22.
//
#include <__sympl_old/include/Parser/Node/VarAssignNode.hpp>
#include <__sympl_old/include/Parser/Token.hpp>
#include "__sympl_old/include/Parser/LexerPosition.hpp"
SymplNamespace

void VarAssignNode::__Construct(int argc, va_list ArgList)
{
    if (argc > 0) {
        SharedPtr<Token> pToken = va_arg(ArgList, Token*);
        SharedPtr<ParserNode> pValue = va_arg(ArgList, ParserNode*);

        Create(pToken, pValue);
    }
}

void VarAssignNode::__Destruct()
{
    ParserNode::__Destruct();

    Value = nullptr;
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
