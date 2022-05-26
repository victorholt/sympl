//
// GameSencha, LLC 5/24/22.
//
#include "InvalidSyntaxError.hpp"
#include <fmt/format.h>
SymplNamespace

InvalidSyntaxError::InvalidSyntaxError(size_t StartPosition, size_t EndPosition, CStrPtr ErrorDetails)
	: SymplError(fmt::format("Invalid Syntax Error at {0},{1}", StartPosition, EndPosition).c_str(), ErrorDetails)
{
}
