//
// GameSencha, LLC 6/2/22.
//
#include <__sympl_old/include/Parser/Node/ParserStringNode.hpp>
#include <__sympl_old/include/Parser/Token.hpp>
SymplNamespace

void ParserStringNode::__Construct(int argc, va_list ArgList)
{
    SharedPtr<Token> pToken = va_arg(ArgList, Token*);

    Create(pToken);
}

void ParserStringNode::__Destruct()
{
    ParserNode::__Destruct();
}

void ParserStringNode::Create(const SharedPtr<Token>& pNodeToken)
{
    ParserNode::Create(ParseNodeType::String, pNodeToken);
}
