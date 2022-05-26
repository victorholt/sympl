//
// Created by Victor on 5/24/2022.
//
#include "Parser.hpp"
#include "Token.hpp"
#include "LexerPosition.hpp"
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
    auto Result = Expression();
    assert(Result.IsValid());

    if (Result->Error.IsValid() || CurrentToken->GetType() != TokenType::EndOfFile)
    {
        Result->Failure(new InvalidSyntaxError(
                CurrentToken->GetStartPosition()->GetLineNumber(),
                CurrentToken->GetEndPosition()->GetLineCol(),
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
        case TokenType::Int:
        case TokenType::Float:
            Advance();
            Result->Success(new ParserNumberNode(FactorToken));
			return Result;
    }

	Result->Failure(new InvalidSyntaxError(
            FactorToken->GetStartPosition()->GetLineNumber(),
            FactorToken->GetEndPosition()->GetLineCol(),
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
	return BinaryOperation(
            [=] { return Term(); },
            { TokenType::Plus, TokenType::Minus }
    );
}
