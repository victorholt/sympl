//
// GameSencha, LLC 6/3/22.
//
#include "ParserAccessScopeNode.hpp"
#include "sympl/Parser/Token.hpp"
SymplNamespace

void ParserAccessScopeNode::__Construct(int argc, va_list ArgList)
{
    SharedPtr<Token> pToken = va_arg(ArgList, Token*);
    SharedPtr<Token> pParentScopeToken = va_arg(ArgList, Token*);

    Create(pToken, pParentScopeToken);
}

void ParserAccessScopeNode::Create(const SharedPtr<Token>& pNodeToken, const SharedPtr<Token> pParentScopeToken)
{
    ParserNode::Create(ParseNodeType::ScopeAccess, pNodeToken);
    ParentScopeToken = pParentScopeToken;
}
