//
// GameSencha, LLC 5/26/22.
//
#include "RuntimeError.hpp"
#include "sympl/Core/StringBuffer.hpp"
#include "sympl/Parser/ParserContext.hpp"
#include "sympl/Parser/LexerPosition.hpp"
#include "sympl/thirdparty/fmt/format.h"
SymplNamespace

RuntimeError::RuntimeError(
        const SharedPtr<ParserContext>& pContext,
        const SharedPtr<LexerPosition>& pStartPosition,
        const SharedPtr<LexerPosition>& pEndPosition,
        CStrPtr ErrorDetails)
        : ParserError(fmt::format(
        "Runtime Error at Line {0} ({1}, {2})",
        pStartPosition->GetLineNumber(),
        pStartPosition->GetLineCol(),
        pEndPosition->GetLineCol()
).c_str(), ErrorDetails)
{
    Context = pContext;
    StartPosition = pStartPosition;
    EndPosition = pEndPosition;
}

SharedPtr<struct StringBuffer> RuntimeError::GenerateTraceback()
{
    auto Result = StringBuffer::Alloc<StringBuffer>();
    auto CurrentContext = Context;
    auto CurrentPosition = StartPosition;


    while (CurrentContext.IsValid())
    {
        Result->Append(fmt::format(
            "File {0}, Line {1}, in {2}\n",
            CurrentPosition->GetFileName(),
            CurrentPosition->GetLineNumber(),
            CurrentContext->DisplayName->CStr()
        ).c_str());

        if (!CurrentContext->Parent.IsValid())
        {
            break;
        }

        CurrentContext = CurrentContext->Parent;
        CurrentPosition = CurrentContext->ParentEntryPosition;
    }

    Result->Prepend("Parser Traceback:\n");
    return Result;
}

CStrPtr RuntimeError::ToString()
{
    ErrorMessage->Clear();
    ErrorMessage->Append(GenerateTraceback()->CStr());
    ErrorMessage->Append(fmt::format("<{0}>: {1}", ErrorName->CStr(), ErrorDetails->CStr()).c_str());

    return ErrorMessage->CStr();
}
