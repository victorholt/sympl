//
// GameSencha, LLC 5/24/22.
//
#pragma once
#include "SymplPCH.hpp"
#include "ParserNode.hpp"
#include "ParserBinaryNode.hpp"

SymplNamespaceStart

union ParserNodeObject {
    ParserNode BaseNode;
    ParserBinaryNode BinaryNode;

    inline ParserNodeObject& operator=(const ParserNodeObject& rhs) {
        if (this == &rhs) {
            return *this;
        }

        BaseNode = rhs.BaseNode;
        BinaryNode = rhs.BinaryNode;

        return *this;
    }
};

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

    ParserNodeObject Parse();

    /**
     * Returns the current token.
     * @return
     */
    class Token* GetCurrentToken();

    char* ToString() const;

protected:

    /**
     * Advances the parser.
     */
    class Token* Advance();

    /**
     * Returns a node based on a factor.
     * @return
     */
    ParserNodeObject Factor();

    /**
     * Returns a node based on a term.
     * @return
     */
    ParserNodeObject Term();

    /**
     * Returns a node based on a binary operation.
     * @param OpMethod
     * @param ValidOps
     * @return
     */
    ParserNodeObject BinaryOperation(std::function<ParserNodeObject()> OpMethod, const std::vector<TokenType>& ValidOps);

    /**
     * Returns the node based on a given expression.
     * @return
     */
    ParserNodeObject Expression();

};

SymplNamespaceEnd
