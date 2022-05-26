//
// GameSencha, LLC 5/26/22.
//
#include "RuntimeError.hpp"
#include "sympl/Parser/LexerPosition.hpp"
#include "sympl/thirdparty/fmt/format.h"
SymplNamespace

RuntimeError::RuntimeError(const SharedPtr<LexerPosition>& StartPosition, const SharedPtr<LexerPosition>& EndPosition, CStrPtr ErrorDetails)
        : ParserError(fmt::format(
        "Runtime Error at Line {0} ({1}, {2})",
        StartPosition->GetLineNumber(),
        StartPosition->GetLineCol(),
        EndPosition->GetLineCol()
).c_str(), ErrorDetails)
{
}
