//
// GameSencha, LLC 5/24/22.
//
#pragma once

#include "sympl/SymplPCH.hpp"
#include <sympl/Memory/SharedPtr.hpp>

SymplNamespaceStart

class Lexer
{
private:
	// List of tokens.
	std::vector<SharedPtr<class Token>> TokenList;

	// List of errors.
	std::vector<SharedPtr<class ParserError>> ErrorList;

	// Buffer of text to iterate through.
	SharedPtr<class StringBuffer> Buffer;

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
	SharedPtr<class Token> MakeNumberToken();

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
	inline const std::vector<SharedPtr<class Token>>& GetTokens() const { return TokenList; }

	/**
	 * Returns the list of errors.
	 * @return
	 */
	inline const std::vector<SharedPtr<class ParserError>>& GetErrors() const { return ErrorList; }

};

SymplNamespaceEnd