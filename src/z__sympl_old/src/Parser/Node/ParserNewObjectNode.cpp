//
// GameSencha, LLC 6/3/22.
//
#include <__sympl_old/include/Parser/Node/ParserNewObjectNode.hpp>
#include <__sympl_old/include/Parser/Token.hpp>
SymplNamespace

void ParserNewObjectNode::__Construct(int argc, va_list ArgList)
{
    SharedPtr<Token> pToken = va_arg(ArgList, Token*);

    Create(pToken);
}

void ParserNewObjectNode::Create(
        const SharedPtr<Token>& pNodeToken
)
{
    ParserNode::Create(ParseNodeType::NewObject, pNodeToken);
}
