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
	 * @param StartPosition
	 * @param EndPosition
	 * @param ErrorDetails
	 */
	InvalidSyntaxError(size_t StartPosition, size_t EndPosition, CStrPtr ErrorDetails);
};

SymplNamespaceEnd
