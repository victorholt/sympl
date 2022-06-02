//
// GameSencha, LLC 6/2/22.
//
#include "ParserStringNode.hpp"
#include "sympl/Parser/Token.hpp"
SymplNamespace

void ParserStringNode::__Construct(int argc, va_list ArgList)
{
    SharedPtr<Token> pToken = va_arg(ArgList, Token*);

    Create(pToken);
}

void ParserStringNode::Create(const SharedPtr<Token>& pNodeToken)
{
    ParserNode::Create(ParseNodeType::String, pNodeToken);
}
