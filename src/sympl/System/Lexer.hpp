//
// GameSencha, LLC 5/24/22.
//
#pragma once

#include "SymplPCH.hpp"

SymplNamespaceStart

class Lexer
{
private:
	// List of tokens.
	std::vector<struct Token> TokenList;

	// List of errors.
	std::vector<class SymplError> ErrorList;

	// Buffer of text to iterate through.
	StrPtr Buffer;

	// Current character.
	char CurrentChar;

	// Current buffer position.
	size_t BufferPosition;

	// Digits to check for.
	std::vector<char> Digits;

protected:

	/**
	 * Generates a number.
	 */
	struct Token MakeNumberToken();

public:

	/**
	 * Constructor.
	 * @param Text
	 */
	Lexer(CStrPtr Text);

	/**
	 * Generates a list of tokens from the buffer.
	 * @param TokenListOutput
	 */
	void MakeTokens();

	/**
	 * Advances the lexer to the next character.
	 */
	void Advance();

	/**
	 * Returns the list of tokens.
	 * @return
	 */
	inline const std::vector<struct Token>& GetTokens() const { return TokenList; }

	/**
	 * Returns the list of errors.
	 * @return
	 */
	inline const std::vector<class SymplError>& GetErrors() const { return ErrorList; }

};

SymplNamespaceEnd
