//
// Created by Victor on 5/24/2022.
//
#include "Parser.hpp"
#include "Token.hpp"
#include "InvalidSyntaxError.hpp"
SymplNamespace

Parser::Parser(const std::vector<class Token>& pTokenList)
    : TokenIndex(0), CurrentToken(nullptr)
{
    TokenList = pTokenList;
    Advance();
}

SharedPtr<ParseResult> Parser::Parse()
{
    return Expression();
}

void Parser::Advance()
{
    if (TokenIndex >= TokenList.size()) {
		return;
	}

	CurrentToken = &TokenList[TokenIndex++];
}

SharedPtr<ParseResult> Parser::Factor()
{
	SharedPtr<ParseResult> Result = SharedPtr<ParseResult>(new ParseResult());
	Token* FactorToken = CurrentToken;

	switch (FactorToken->GetType())
    {
        case TokenType::Int:
        case TokenType::Float:
            Advance();
            Result->Success(new ParserNumberNode(FactorToken));
			return Result;
    }

	Result->Failure(new InvalidSyntaxError(FactorToken->ToString()));

	return Result;
}

SharedPtr<ParseResult> Parser::Term()
{
	SharedPtr<ParseResult> Result = SharedPtr<ParseResult>(new ParseResult());
    return BinaryOperation(
        [=] { return Factor(); },
        { TokenType::Mul, TokenType::Div }
    );
}

SharedPtr<ParseResult> Parser::BinaryOperation(std::function<SharedPtr<ParseResult>()> OpMethod, const std::vector<TokenType>& ValidOps)
{
	SharedPtr<ParseResult> Result = SharedPtr<ParseResult>(new ParseResult());
	auto LeftNodeResult = OpMethod();

	assert(!LeftNodeResult->Error.IsValid());

    while (std::find(ValidOps.begin(), ValidOps.end(), CurrentToken->GetType()) != ValidOps.end())
    {
        Token* OpToken = CurrentToken;
        Advance();

        auto RightNodeResult = OpMethod();
		assert(RightNodeResult->Error.IsValid());

		auto LeftNode = new ParserBinaryNode(
			static_cast<ParserNumberNode*>(LeftNodeResult->ParserNode.Ptr()),
			OpToken,
			static_cast<ParserNumberNode*>(RightNodeResult->ParserNode.Ptr())
		);

		LeftNodeResult->Success(LeftNode);
    }

    return LeftNodeResult;
}

SharedPtr<ParseResult> Parser::Expression()
{
	SharedPtr<ParseResult> Result = SharedPtr<ParseResult>(new ParseResult());
    return BinaryOperation(
            [=] { return Term(); },
            { TokenType::Plus, TokenType::Minus }
    );
}
