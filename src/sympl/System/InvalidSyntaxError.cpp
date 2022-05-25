//
// GameSencha, LLC 5/24/22.
//
#include "InvalidSyntaxError.hpp"
SymplNamespace

InvalidSyntaxError::InvalidSyntaxError(CStrPtr ErrorDetails)
	: SymplError("Invalid Syntax Error", ErrorDetails)
{
}
