//
// GameSencha, LLC 5/24/22.
//
#pragma once

#include "SymplPCH.hpp"

SymplNamespaceStart

class Token
{
private:

	// Our token type base on the list of token types.
	TokenType Type;

	// Current value of our token.
	std::shared_ptr<StrPtr> Value;

	// Temporary storage for holding the to string value.
	char TmpAlloc_ToString[512];

public:

	/**
	 * Constructor.
	 * @param Type
	 * @param ValueStr
	 */
	Token(TokenType Type, CStrPtr ValueStr);

    /**
     * Constructor.
     * @param CopyToken
     */
    Token(Token* CopyToken);

	/**
	 * Prints a nice representation of the token.
	 * @return
	 */
	CStrPtr ToString();

    /**
     * Copies from another token.
     * @param rhs
     * @return
     */
	Token& operator = (const Token& rhs);

    /**
     * Returns the type of the token.
     * @return
     */
    inline TokenType GetType() const { return Type; }

    /**
     * Returns the token value.
     * @return
     */
    inline CStrPtr GetValue() const { return *Value; }
};

SymplNamespaceEnd
