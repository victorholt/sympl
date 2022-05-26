//
// Created by Victor on 5/24/2022.
//
#include "Parser.hpp"
#include "Token.hpp"
#include "LexerPosition.hpp"
#include "sympl/Parser/Error/InvalidSyntaxError.hpp"
#include "sympl/Parser/Node/ParserUnaryOpNode.hpp"
SymplNamespace

Parser::Parser(const std::vector<class Token>& pTokenList)
    : TokenIndex(0), CurrentToken(nullptr)
{
    TokenList = pTokenList;
    Advance();
}

SharedPtr<ParseResult> Parser::Parse()
{
    auto Result = Expression();
    assert(Result.IsValid());

    if (Result->Error.IsValid() || CurrentToken->GetType() != TokenType::EndOfFile)
    {
        Result->Failure(new InvalidSyntaxError(
                CurrentToken->GetStartPosition(),
                CurrentToken->GetEndPosition(),
                fmt::format("Expected '+', '-', '*' or '/', got {0}", CurrentToken->ToString()).c_str()
        ));
    }

    return Result;
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
        case TokenType::Plus:
        case TokenType::Minus: {
            Advance();
            SharedPtr<ParseResult> ResultFactor = Factor();
            if (ResultFactor->Error.IsValid()) {
                return ResultFactor;
            }

            Result->Success(new ParserUnaryOpNode(FactorToken, ResultFactor->ParserNodePtr));
            return Result;
        }

        case TokenType::Int:
        case TokenType::Float: {
            Advance();
            Result->Success(new ParserNumberNode(FactorToken));
            return Result;
        }

        case TokenType::LH_Parenth:
        {
            Advance();

            // Recursively evaluate the expression.
            auto Expr = Expression();
            if (Expr->Error.IsValid()) {
                return Expr;
            }

            // We should end at the right parenth.
            if (CurrentToken->GetType() == TokenType::RH_Parenth)
            {
                Advance();
                return Expr;
            }
            else
            {
                Expr->Failure(new InvalidSyntaxError(
                        FactorToken->GetStartPosition(),
                        FactorToken->GetEndPosition(),
                        fmt::format("Expected ')', got {0}", FactorToken->ToString()).c_str()
                ));
                return Expr;
            }
        }
    }

	Result->Failure(new InvalidSyntaxError(
            FactorToken->GetStartPosition(),
            FactorToken->GetEndPosition(),
            fmt::format("Expected int or float, got {0}", FactorToken->ToString()).c_str()
    ));

	return Result;
}

SharedPtr<ParseResult> Parser::Term()
{
	return BinaryOperation(
        [=] { return Factor(); },
        { TokenType::Mul, TokenType::Div }
    );
}

SharedPtr<ParseResult> Parser::BinaryOperation(std::function<SharedPtr<ParseResult>()> OpMethod, const std::vector<TokenType>& ValidOps)
{
	SharedPtr<ParseResult> Result = SharedPtr<ParseResult>(new ParseResult());
	auto LeftNodeResult = OpMethod();

    if (LeftNodeResult->Error.IsValid()) {
        return LeftNodeResult;
    }

    while (std::find(ValidOps.begin(), ValidOps.end(), CurrentToken->GetType()) != ValidOps.end())
    {
        Token* OpToken = CurrentToken;
        Advance();

        auto RightNodeResult = OpMethod();
        if (RightNodeResult->Error.IsValid()) {
            return RightNodeResult;
        }

		auto LeftNode = new ParserBinaryOpNode(
			static_cast<ParserNode*>(LeftNodeResult->ParserNodePtr.Ptr()),
			OpToken,
			static_cast<ParserNode*>(RightNodeResult->ParserNodePtr.Ptr())
		);

		LeftNodeResult->Success(LeftNode);
    }

    return LeftNodeResult;
}

SharedPtr<ParseResult> Parser::Expression()
{
	return BinaryOperation(
            [=] { return Term(); },
            { TokenType::Plus, TokenType::Minus }
    );
}
