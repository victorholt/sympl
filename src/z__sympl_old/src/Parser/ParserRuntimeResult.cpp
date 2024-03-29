//
// GameSencha, LLC 5/26/22.
//
#include <__sympl_old/include/Parser/ParserRuntimeResult.hpp>
#include <__sympl_old/include/Parser/Node/ParserNode.hpp>
#include <__sympl_old/include/Parser/Handle/ValueHandle.hpp>
SymplNamespace

void ParserRuntimeResult::__Destruct()
{
    Error = nullptr;
    Value = nullptr;
    FuncReturnValue = nullptr;
}

SharedPtr<ValueHandle> ParserRuntimeResult::Register(const SharedPtr<ParserRuntimeResult>& pResult)
{
    Error = pResult->Error;
    Value = pResult->Value;
    FuncReturnValue = pResult->FuncReturnValue;
    LoopShouldContinue = pResult->LoopShouldContinue;
    LoopShouldBreak = pResult->LoopShouldBreak;

    return Value;
}

void ParserRuntimeResult::Success(const SharedPtr<ValueHandle>& pValue)
{
    Reset();
    Value = pValue;
}

void ParserRuntimeResult::SuccessReturn(const SharedPtr<struct ValueHandle>& pValue)
{
    Reset();
    FuncReturnValue = pValue;
}

void ParserRuntimeResult::SuccessContinue()
{
    Reset();
    LoopShouldContinue = true;
}

void ParserRuntimeResult::SuccessBreak()
{
    Reset();
    LoopShouldBreak = true;
}

void ParserRuntimeResult::Failure(const SharedPtr<ParserError>& pError)
{
    Reset();
    Error = pError;
}

bool ParserRuntimeResult::ShouldReturn()
{
    return (
        Error.IsValid() ||
        FuncReturnValue.IsValid() ||
        LoopShouldContinue ||
        LoopShouldBreak
    );
}

void ParserRuntimeResult::Reset()
{
    Error = nullptr;
    Value = nullptr;
    FuncReturnValue = nullptr;
    LoopShouldContinue = false;
    LoopShouldBreak = false;
}
