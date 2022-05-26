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
	InvalidSyntaxError(const SharedPtr<class LexerPosition>& StartPosition, const SharedPtr<class LexerPosition>& EndPosition, CStrPtr ErrorDetails);
};

SymplNamespaceEnd
