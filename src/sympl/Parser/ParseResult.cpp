//
// GameSencha, LLC 5/25/22.
//
#include "ParseResult.hpp"
#include "sympl/Parser/Node/ParserNode.hpp"
SymplNamespace

void ParseResult::Success(const SharedPtr<ParserNode>& Node)
{
    ParserNodePtr = Node;
}

void ParseResult::Failure(const SharedPtr<ParserError>& pError)
{
	Error = pError;
}
