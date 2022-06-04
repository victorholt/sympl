//
// GameSencha, LLC 6/3/22.
//
#include "ParserObjectNode.hpp"
#include "sympl/Parser/Token.hpp"
SymplNamespace

void ParserObjectNode::__Construct(int argc, va_list ArgList)
{
    SharedPtr<Token> pToken = va_arg(ArgList, Token*);
    SharedPtr<ParserNode> pBodyNode = va_arg(ArgList, ParserNode*);

    Create(pToken, pBodyNode);
}

void ParserObjectNode::Create(
    const SharedPtr<class Token>& pNodeToken,
    const SharedPtr<ParserNode>& pBodyNode
)
{
    ParserNode::Create(ParseNodeType::Object, pNodeToken);
    BodyNode = pBodyNode;
}
