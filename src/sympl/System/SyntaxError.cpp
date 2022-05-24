//
// GameSencha, LLC 5/24/22.
//
#include "SyntaxError.hpp"
SymplNamespace

SyntaxError::SyntaxError(CStrPtr ErrorDetails)
	: SymplError("Syntax Error", ErrorDetails)
{
}
