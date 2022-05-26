//
// GameSencha, LLC 5/24/22.
//
#include "InvalidSyntaxError.hpp"
#include "sympl/Parser/LexerPosition.hpp"
#include "sympl/thirdparty/fmt/format.h"
SymplNamespace

InvalidSyntaxError::InvalidSyntaxError(const SharedPtr<LexerPosition>& StartPosition, const SharedPtr<LexerPosition>& EndPosition, CStrPtr ErrorDetails)
	: ParserError(fmt::format(
            "Invalid Syntax Error at Line {0} ({1}, {2})",
            StartPosition->GetLineNumber(),
            StartPosition->GetLineCol(),
            EndPosition->GetLineCol()
    ).c_str(), ErrorDetails)
{
}