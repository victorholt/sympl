//
// GameSencha, LLC 5/24/22.
//
#pragma once
#include "SymplPCH.hpp"
#include "ParserNumberNode.hpp"
#include "ParserBinaryNode.hpp"
#include "ParseResult.hpp"

SymplNamespaceStart

class Parser
{
private:
    // Current token index.
    size_t TokenIndex;

    // List of tokens to iterate through.
    std::vector<class Token> TokenList;

    // Reference to the current token.
    class Token* CurrentToken;

public:

    /**
     * Constructor.
     * @param TokenList
     */
    Parser(const std::vector<class Token>& pTokenList);

	/**
	 * Parses the tokens.
	 * @return
	 */
	SharedPtr<ParseResult> Parse();

protected:

    /**
     * Advances the parser.
     */
    void Advance();

    /**
     * Returns a node based on a factor.
     * @return
     */
	SharedPtr<ParseResult> Factor();

    /**
     * Returns a node based on a term.
     * @return
     */
	SharedPtr<ParseResult> Term();

    /**
     * Returns a node based on a binary operation.
     * @param OpMethod
     * @param ValidOps
     * @return
     */
	SharedPtr<ParseResult> BinaryOperation(std::function<SharedPtr<ParseResult>()> OpMethod, const std::vector<TokenType>& ValidOps);

    /**
     * Returns the node based on a given expression.
     * @return
     */
	SharedPtr<ParseResult> Expression();

};

SymplNamespaceEnd
