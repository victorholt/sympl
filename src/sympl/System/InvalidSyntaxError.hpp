//
// GameSencha, LLC 5/24/22.
//
#pragma once
#include "SymplError.hpp"

SymplNamespaceStart

class InvalidSyntaxError : public SymplError
{
public:

	/**
	 * Constructor.
	 * @param ErrorDetails
	 */
	InvalidSyntaxError(CStrPtr ErrorDetails);
};

SymplNamespaceEnd
