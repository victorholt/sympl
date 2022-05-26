//
// GameSencha, LLC 5/25/22.
//
#include "ParseResult.hpp"
SymplNamespace

void ParseResult::Success(const SharedPtr<ManagedObject>& Node)
{
	ParserNode = Node;
}

void ParseResult::Failure(const SharedPtr<SymplError>& pError)
{
	Error = pError;
}
