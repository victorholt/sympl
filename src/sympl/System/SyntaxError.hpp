//
// GameSencha, LLC 5/24/22.
//
#pragma once
#include "SymplError.hpp"

SymplNamespaceStart

class SyntaxError : public SymplError
{
public:

	/**
	 * Constructor.
	 * @param ErrorDetails
	 */
	SyntaxError(CStrPtr ErrorDetails);
};

SymplNamespaceEnd
