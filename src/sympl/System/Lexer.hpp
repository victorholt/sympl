//
// GameSencha, LLC 5/24/22.
//
#pragma once

#include "SymplPCH.hpp"
#include <sympl/Memory/SharedPtr.hpp>

SymplNamespaceStart

class Lexer
{
private:
	// List of tokens.
	std::vector<class Token> TokenList;

	// List of errors.
	std::vector<class SymplError> ErrorList;

	// Buffer of text to iterate through.
	StrPtr Buffer;

	// Current character.
	char CurrentChar;

	// Current buffer position.
	SharedPtr<class LexerPosition> Position;

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
	 * @param FileName
	 * @param Text
	 */
	Lexer(CStrPtr FileName, CStrPtr Text);

    /**
     * Destructor.
     */
    ~Lexer();

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
	inline const std::vector<class Token>& GetTokens() const { return TokenList; }

	/**
	 * Returns the list of errors.
	 * @return
	 */
	inline const std::vector<class SymplError>& GetErrors() const { return ErrorList; }

};

SymplNamespaceEnd
