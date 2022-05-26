//
// GameSencha, LLC 5/26/22.
//
#include "ParserRuntimeResult.hpp"
#include "sympl/Parser/Node/ParserNode.hpp"
#include "sympl/Parser/Handle/ValueHandle.hpp"
SymplNamespace

SharedPtr<ValueHandle> ParserRuntimeResult::Register(const SharedPtr<ParserRuntimeResult>& pResult)
{
    Value = pResult->Value;
    if (pResult->Error.IsValid())
    {
        Error = pResult->Error;
    }

    return Value;
}

void ParserRuntimeResult::Success(const SharedPtr<ValueHandle>& pValue)
{
    Value = pValue;
}

void ParserRuntimeResult::Failure(const SharedPtr<ParserError>& pError)
{
    Error = pError;
}
