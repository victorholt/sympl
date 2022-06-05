//
// GameSencha, LLC 6/3/22.
//
#include <sympl/include/Parser/Node/ParserScopeAccessNode.hpp>
#include <sympl/include/Parser/Token.hpp>
SymplNamespace

void ParserScopeAccessNode::__Construct(int argc, va_list ArgList)
{
    SharedPtr<Token> pToken = va_arg(ArgList, Token*);
    SharedPtr<Token> pParentScopeToken = va_arg(ArgList, Token*);

    Create(pToken, pParentScopeToken);
}

void ParserScopeAccessNode::Create(
    const SharedPtr<Token>& pNodeToken,
    const SharedPtr<Token>& pParentScopeToken
)
{
    ParserNode::Create(ParseNodeType::ScopeAccess, pNodeToken);
    ParentScopeToken = pParentScopeToken;
}
