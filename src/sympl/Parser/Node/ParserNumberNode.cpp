//
// GameSencha, LLC 5/24/22.
//
#include "ParserNumberNode.hpp"
#include "sympl/Parser/Token.hpp"
SymplNamespace

ParserNumberNode::ParserNumberNode(Token* pNodeToken)
    : ParserNode(ParseNodeType::Number, pNodeToken)
{
}
