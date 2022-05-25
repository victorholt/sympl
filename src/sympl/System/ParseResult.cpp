//
// GameSencha, LLC 5/25/22.
//
#include "ParseResult.hpp"
SymplNamespace

SharedPtr<ManagedObject> ParseResult::RegisterResult(SharedPtr<ParseResult> Result)
{
	if (Result->Error.IsValid()) {
		Error = Result->Error;
	}
	return Result->ParserNode;
}

SharedPtr<ManagedObject> ParseResult::RegisterNode(SharedPtr<ManagedObject> Node)
{
	return Node;
}

void ParseResult::Success(const SharedPtr<ManagedObject>& Node)
{
	ParserNode = Node;
}

void ParseResult::Failure(const SharedPtr<SymplError>& pError)
{
	Error = pError;
}
