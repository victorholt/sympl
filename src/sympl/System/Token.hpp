//
// GameSencha, LLC 5/24/22.
//
#pragma once

#include "SymplPCH.hpp"

SymplNamespaceStart

// Base token types.
enum class TokenType
{
	Int = 0,
	Float,
	Plus,
	Minus,
	Mul,
	Div,
	LH_Parenth,
	RH_Parenth
};

struct Token
{
	// Our token type base on the list of token types.
	TokenType Type;
	// Current value of our token.
	CStrPtr Value;

	// Temporary storage for holding the to string value.
	char TmpAlloc_ToString[512];

	/**
	 * Constructor.
	 * @param Type
	 * @param Value
	 */
	Token(TokenType Type, CStrPtr Value);

	/**
	 * Prints a nice representation of the token.
	 * @return
	 */
	CStrPtr ToString();

	Token& operator = (const Token& rhs);
};

SymplNamespaceEnd