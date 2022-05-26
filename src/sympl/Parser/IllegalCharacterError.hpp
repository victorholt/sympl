//
// GameSencha, LLC 5/25/22.
//
#pragma once
#include "SymplError.hpp"

SymplNamespaceStart

class IllegalCharacterError : public SymplError
{
public:

	/**
	 * Constructor.
	 * @param ErrorDetails
	 */
	IllegalCharacterError(CStrPtr ErrorDetails);
};

SymplNamespaceEnd
