//
// GameSencha, LLC 5/24/22.
//
#pragma once

#include <sympl/SymplPCH.hpp>
#include <sympl/include/Memory/SharedPtr.hpp>
#include <sympl/include/Memory/ManagedObject.hpp>

SymplNamespaceStart

class SYMPL_API Lexer : public ManagedObject
{
	SYMPL_OBJECT(Lexer, ManagedObject)

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
    // Valid letters for the lexer.
	std::vector<char> Letters;

protected:

    /**
     * Destructor.
     */
    void __Destruct() override;

    /**
     * Attempt to skip a comment.
     */
    void SkipComment();

	/**
	 * Generates a number.
	 */
	SharedPtr<class Token> MakeNumberToken();

    /**
     * Generates an identifier token.
     * @return
     */
    SharedPtr<class Token> MakeIdentifier();

    /**
     * Generates a string token.
     * @return
     */
    SharedPtr<class Token> MakeString();

    /**
     * Make either a minus or arrow token.
     * @return
     */
    SharedPtr<class Token> MakeMinusOrArrow();

    /**
     * Make the not equals token.
     * @return
     */
    SharedPtr<class Token> MakeNotEquals();

    /**
     * Make the equals token.
     * @return
     */
    SharedPtr<class Token> MakeEquals();

    /**
     * Make the greater than token.
     * @return
     */
    SharedPtr<class Token> MakeGreaterThan();

    /**
     * Make the less than token.
     * @return
     */
    SharedPtr<class Token> MakeLessThan();

	/**
	 * Make an or token.
	 * @return
	 */
	SharedPtr<class Token> MakeOrOp();

	/**
	 * Make an and token.
	 * @return
	 */
	SharedPtr<class Token> MakeAndOp();

    /**
     * Reference to the error.
     */
    SharedPtr<class ParserError> CreateIllegalCharacterError();

    /**
     * Reference to the error.
     */
    SharedPtr<class ParserError> CreateExpectedCharacterError(CStrPtr ExpectedCharacter, CStrPtr AfterCharacter);

public:

	/**
	 * Constructor.
	 * @param FileName
	 * @param Text
	 */
	void Create(CStrPtr FileName, CStrPtr Text);

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
