//
// GameSencha, LLC 5/24/22.
//
#pragma once
#include <__sympl_old/SymplPCH.hpp>
#include <__sympl_old/include/Parser/Node/ParserNumberNode.hpp>
#include <__sympl_old/include/Parser/Node/ParserBinaryOpNode.hpp>
#include "ParseResult.hpp"

SymplNamespaceStart

class SYMPL_API Parser : public ManagedObject
{
	SYMPL_OBJECT(Parser, ManagedObject)

private:
    // Current token index.
    long long TokenIndex;

    // List of tokens to iterate through.
    std::vector<SharedPtr<Token>> TokenList;

    // Reference to the current token.
    SharedPtr<Token> CurrentToken;

public:

    /**
     * Destructor.
     */
    void __Destruct() override;

    /**
     * Creates the parser.
     * @param TokenList
     */
    void Create(const std::vector<SharedPtr<class Token>>& pTokenList);

	/**
	 * Parses the tokens.
	 * @return
	 */
	SharedPtr<ParseResult> Parse();

protected:

    /**
     * Advances the parser.
     */
    class Token* Advance();

    /**
     * Reverse the parser advancement by a given amount.
     * @param Amount
     */
    class Token* Reverse(int Amount);

    /**
     * Retrieve the previous token.
     * @return
     */
    class Token* Previous(int Amount = 1);

    /**
     * Peek ahead to get the next token.
     * @return
     */
    class Token* Peek(int Amount = 1);

    /**
     * Safely check and update the current token value.
     */
    void UpdateCurrentToken();

    /**
     * Returns the result based on a given expression.
     * @return
     */
    SharedPtr<ParseResult> Power();

    /**
     * Returns a result based on call rules.
     * @return
     */
    SharedPtr<ParseResult> CallFunc();

    /**
     * Returns a result based on call rules.
     * @return
     */
    SharedPtr<ParseResult> CallFuncFromAtom(SharedPtr<ParserNode> AtomNode);

    /**
     * Returns a result based on an atom rule.
     * @return
     */
    SharedPtr<ParseResult> Atom();

    /**
     * Returns a result based on a factor.
     * @return
     */
	SharedPtr<ParseResult> Factor();

    /**
     * Returns a result based on a term.
     * @return
     */
	SharedPtr<ParseResult> Term();

    /**
     * Returns a result based on a binary operation.
     * @param OpMethod
     * @param ValidOps
     * @return
     */
	SharedPtr<ParseResult> BinaryOperation(
        const std::function<SharedPtr<ParseResult>()>& LeftOpMethod,
		const std::unordered_map<TokenType, std::string>& ValidOps,
        const std::function<SharedPtr<ParseResult>()>& RightOpMethod = nullptr
    );

    /**
     * Returns an array of statements as the parse result.
     * @return
     */
    SharedPtr<ParseResult> Statements();

    /**
     * Returns a single statement as the parse result.
     * @return
     */
    SharedPtr<ParseResult> Statement();

    /**
     * Returns the result based on a given expression.
     * @return
     */
	SharedPtr<ParseResult> Expression();

	/**
	 * Returns a result based on a compare expression.
	 * @return
	 */
	SharedPtr<ParseResult> CompExpr();

	/**
	 * Returns a result based on an arithmetic expression.
	 * @return
	 */
	SharedPtr<ParseResult> ArithExpr();

    /**
	 * Returns a result based on an list expression.
	 * @return
	 */
    SharedPtr<ParseResult> ListExpr();

	/**
	 * Returns a result based on an if expression.
	 * @return
	 */
	SharedPtr<ParseResult> IfExpr();

    /**
     * Returns cases of if expressions.
     * @param CaseKeyword
     * @return
     */
    SharedPtr<ParseResult> IfExprCases(CStrPtr CaseKeyword);

    /**
     * Returns cases of if expressions.
     * @param CaseKeyword
     * @return
     */
    SharedPtr<ParseResult> IfExprB();

    /**
     * Returns cases of if expressions.
     * @param CaseKeyword
     * @return
     */
    SharedPtr<ParseResult> IfExprC();

    /**
     * Returns cases of if expressions.
     * @param CaseKeyword
     * @return
     */
    SharedPtr<ParseResult> IfExprBOrC();

	/**
	 * Returns a result based on a for expression.
	 * @return
	 */
	SharedPtr<ParseResult> ForExpr();

	/**
	 * Returns a result based on a while expression.
	 * @return
	 */
	SharedPtr<ParseResult> WhileExpr();

    /**
     * Returns a result based on an access expression.
     * @param pParentScopeToken
     * @return
     */
    SharedPtr<ParseResult> ScopeAccessExpr(const SharedPtr<class Token>& pParentScopeToken);

    /**
	 * Returns a result based on a function definition.
	 * @return
	 */
    SharedPtr<ParseResult> FuncDef();

    /**
	 * Returns a result based on a object definition.
	 * @return
	 */
    SharedPtr<ParseResult> ObjectDef();

    /**
	 * Returns a result based on a object definition.
	 * @return
	 */
    SharedPtr<ParseResult> NewObjectDef();
};

SymplNamespaceEnd
