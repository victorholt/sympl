//
// GameSencha, LLC 5/25/22.
//
#pragma once
#include "ParserError.hpp"

SymplNamespaceStart

class IllegalCharacterError : public ParserError
{
public:

	/**
	 * Constructor.
	 * @param ErrorDetails
	 */
	IllegalCharacterError(CStrPtr ErrorDetails);
};

SymplNamespaceEnd
