//
// GameSencha, LLC 5/25/22.
//
#pragma once
#include "ParserError.hpp"

SymplNamespaceStart

class SYMPL_API IllegalCharacterError : public ParserError
{
	SYMPL_OBJECT(IllegalCharacterError, ParserError)

public:

	/**
	 * Constructor.
	 * @param ErrorDetails
	 */
	IllegalCharacterError(CStrPtr ErrorDetails);
};

SymplNamespaceEnd
