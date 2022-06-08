//
// GameSencha, LLC 5/24/22.
//
#include <sympl/include/Parser/Node/ParserNumberNode.hpp>
#include <sympl/include/Parser/Token.hpp>
SymplNamespace

void ParserNumberNode::__Construct(int argc, va_list ArgList)
{
    SharedPtr<Token> pToken = va_arg(ArgList, Token*);

    Create(pToken);
}

void ParserNumberNode::__Destruct()
{
    ParserNode::__Destruct();
}

void ParserNumberNode::Create(const SharedPtr<Token>& pNodeToken)
{
    ParserNode::Create(ParseNodeType::Number, pNodeToken);
}
