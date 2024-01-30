//
// GameSencha, LLC 5/24/22.
//
#pragma once
#include "ParserError.hpp"

SymplNamespaceStart

class SYMPL_API InvalidSyntaxError : public ParserError
{
	SYMPL_OBJECT(InvalidSyntaxError, ParserError)

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
