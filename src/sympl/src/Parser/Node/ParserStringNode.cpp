//
// GameSencha, LLC 6/2/22.
//
#include <sympl/include/Parser/Node/ParserStringNode.hpp>
#include <sympl/include/Parser/Token.hpp>
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
