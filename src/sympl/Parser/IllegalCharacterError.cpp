//
// GameSencha, LLC 5/25/22.
//
#include "IllegalCharacterError.hpp"
SymplNamespace

IllegalCharacterError::IllegalCharacterError(CStrPtr ErrorDetails)
		: SymplError("Illegal Character Error", ErrorDetails)
{
}
