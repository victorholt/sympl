//
// GameSencha, LLC 5/25/22.
//
#include "ParseResult.hpp"
#include "sympl/Parser/Node/ParserNode.hpp"
SymplNamespace

SharedPtr<class ParserNode> ParseResult::Register(const SharedPtr<ParseResult>& Result)
{
    AdvanceCount += Result->AdvanceCount;

    if (Result->Error.IsValid()) {
        Error = Result->Error;
    }

    return Result->ParserNodePtr;
}

SharedPtr<class ParserNode> ParseResult::TryRegister(const SharedPtr<ParseResult>& Result)
{
    if (Result->Error.IsValid()) {
        ToReverseCount = Result->AdvanceCount;
        return nullptr;
    }
    return Register(Result);
}

void ParseResult::RegisterAdvance()
{
    AdvanceCount++;
}

void ParseResult::Success(const SharedPtr<ParserNode>& Node)
{
    ParserNodePtr = Node;
}

void ParseResult::Failure(const SharedPtr<ParserError>& pError)
{
    if (!Error.IsValid() || AdvanceCount == 0) {
        Error = pError;
    }
}
