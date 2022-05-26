//
// GameSencha, LLC 5/24/22.
//
#pragma once

#include "sympl/SymplPCH.hpp"
#include <sympl/Memory/SharedPtr.hpp>

SymplNamespaceStart

class Token
{
private:

	// Our token type base on the list of token types.
	TokenType Type;

	// Current value of our token.
	std::shared_ptr<StrPtr> Value;

	// Start position of the token.
	SharedPtr<class LexerPosition> StartPosition;

	// End position of the token.
    SharedPtr<class LexerPosition> EndPosition;

	// Temporary storage for holding the to string value.
	char TmpAlloc_ToString[512];

public:

	/**
	 * Constructor.
	 * @param Type
	 * @param ValueStr
	 * @param pStartPosition
	 * @param pEndPosition
	 */
	Token(TokenType Type, CStrPtr ValueStr, SharedPtr<LexerPosition> pStartPosition, SharedPtr<LexerPosition> pEndPosition = nullptr);

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

	/**
	 * Returns the start position.
	 * @return
	 */
	inline SharedPtr<LexerPosition> GetStartPosition() const { return StartPosition; }

	/**
	 * Returns the end position.
	 * @return
	 */
	inline SharedPtr<LexerPosition> GetEndPosition() const { return EndPosition; }
};

SymplNamespaceEnd
