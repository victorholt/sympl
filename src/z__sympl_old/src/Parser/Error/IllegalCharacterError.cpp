//
// GameSencha, LLC 5/25/22.
//
#include <__sympl_old/include/Parser/Error/IllegalCharacterError.hpp>
SymplNamespace

IllegalCharacterError::IllegalCharacterError(CStrPtr ErrorDetails)
		: ParserError("Illegal Character Error", ErrorDetails)
{
}
