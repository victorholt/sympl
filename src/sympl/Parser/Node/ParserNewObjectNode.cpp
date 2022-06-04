//
// GameSencha, LLC 6/3/22.
//
#include "ParserNewObjectNode.hpp"
#include "sympl/Parser/Token.hpp"
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
