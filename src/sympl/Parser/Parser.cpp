//
// Created by Victor on 5/24/2022.
//
#include "Parser.hpp"
#include "Token.hpp"
#include "LexerPosition.hpp"
#include "sympl/Parser/Error/InvalidSyntaxError.hpp"
#include "sympl/Parser/Node/ParserUnaryOpNode.hpp"
#include "sympl/Parser/Node/VarAssignNode.hpp"
#include "sympl/Parser/Node/VarAccessNode.hpp"
SymplNamespace

Parser::Parser(const std::vector<SharedPtr<Token>>& pTokenList)
    : TokenIndex(0), CurrentToken(nullptr)
{
    TokenList = pTokenList;
    Advance();
}

SharedPtr<ParseResult> Parser::Parse()
{
    auto Result = Expression();
    assert(Result.IsValid());

    if (!Result->Error.IsValid() && CurrentToken->GetType() != TokenType::EndOfFile)
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

	CurrentToken = TokenList[TokenIndex++].Ptr();
}

SharedPtr<ParseResult> Parser::Atom()
{
    SharedPtr<ParseResult> Result = ParseResult::Alloc<ParseResult>();
    SharedPtr<Token> FactorToken = CurrentToken;

    switch (FactorToken->GetType())
    {
        case TokenType::Int:
        case TokenType::Float: {
            Result->RegisterAdvance();
            Advance();
            Result->Success(ParserNumberNode::Alloc<ParserNumberNode, ParserNode>(1, FactorToken.Ptr()));
            return Result;
        }
        case TokenType::Identifier: {
            Result->RegisterAdvance();
            Advance();
            Result->Success(VarAccessNode::Alloc<VarAccessNode, ParserNode>(1, FactorToken.Ptr()));
            return Result;
        }

        case TokenType::LH_Parenth:
        {
            Result->RegisterAdvance();
            Advance();

            // Recursively evaluate the expression.
            auto Expr = Expression();
            if (Expr->Error.IsValid()) {
                return Expr;
            }

            // We should end at the right parenth.
            if (CurrentToken->GetType() == TokenType::RH_Parenth)
            {
                Result->RegisterAdvance();
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
        fmt::format("Expected int, float, identifier, '+', '-', or '(' got {0}", FactorToken->ToString()).c_str()
    ));

    return Result;
}

SharedPtr<ParseResult> Parser::Factor()
{
	SharedPtr<ParseResult> Result = ParseResult::Alloc<ParseResult>();
	SharedPtr<Token> FactorToken = CurrentToken;

	switch (FactorToken->GetType())
    {
        case TokenType::Plus:
        case TokenType::Minus: {
            Result->RegisterAdvance();
            Advance();
            SharedPtr<ParseResult> ResultFactor = Factor();
            if (ResultFactor->Error.IsValid()) {
                return ResultFactor;
            }

            Result->Success(
                ParserUnaryOpNode::Alloc<ParserUnaryOpNode, ParserNode>(2, FactorToken.Ptr(), ResultFactor->ParserNodePtr.Ptr())
            );
            return Result;
        }
    }

	return Power();
}

SharedPtr<ParseResult> Parser::Term()
{
	return BinaryOperation(
        [=] { return Factor(); },
        { TokenType::Mul, TokenType::Div }
    );
}

SharedPtr<ParseResult> Parser::BinaryOperation(
    const std::function<SharedPtr<ParseResult>()>& LeftOpMethod,
    const std::vector<TokenType>& ValidOps,
    const std::function<SharedPtr<ParseResult>()>& RightOpMethod
)
{
	SharedPtr<ParseResult> Result = ParseResult::Alloc<ParseResult>();
	auto LeftNodeResult = LeftOpMethod();

    if (LeftNodeResult->Error.IsValid()) {
        return LeftNodeResult;
    }

    while (std::find(ValidOps.begin(), ValidOps.end(), CurrentToken->GetType()) != ValidOps.end())
    {
        Token* OpToken = CurrentToken;
        Result->RegisterAdvance();
        Advance();

        auto RightNodeResult = RightOpMethod ? RightOpMethod() : LeftOpMethod();
        if (RightNodeResult->Error.IsValid()) {
            return RightNodeResult;
        }

		auto LeftNode = ParserBinaryOpNode::Alloc<ParserBinaryOpNode, ParserNode>(
            3,
			static_cast<ParserNode*>(LeftNodeResult->ParserNodePtr.Ptr()),
			OpToken,
			static_cast<ParserNode*>(RightNodeResult->ParserNodePtr.Ptr())
		);

		LeftNodeResult->Success(LeftNode);
    }

    return LeftNodeResult;
}

SharedPtr<ParseResult> Parser::Power()
{
    return BinaryOperation(
        [=] { return Atom(); },
        { TokenType::Power },
        [=] { return Factor(); }
    );
}

SharedPtr<ParseResult> Parser::Expression()
{
    SharedPtr<ParseResult> Result = ParseResult::Alloc<ParseResult>();

    if (CurrentToken->GetType() == TokenType::Keyword)
    {
        if (strcmp(CurrentToken->GetValue(), "var") == 0) {
            Result->RegisterAdvance();
            Advance();

            // Advanced and didn't find a proper identifier.
            if (CurrentToken->GetType() != TokenType::Identifier)
            {
                Result->Failure(new InvalidSyntaxError(
                    CurrentToken->GetStartPosition(),
                    CurrentToken->GetEndPosition(),
                    fmt::format("Expected variable identifier, got {0}", CurrentToken->ToString()).c_str()
                ));
                return Result;
            }

            // We found an identifier and we can move forward.
            auto VarName = CurrentToken->Copy();
            Result->RegisterAdvance();
            Advance();

            // Ensure the next token is the set (=) identifier.
            if (CurrentToken->GetType() != TokenType::Equals)
            {
                Result->Failure(new InvalidSyntaxError(
                    CurrentToken->GetStartPosition(),
                    CurrentToken->GetEndPosition(),
                    fmt::format("Expected = token, got {0}", CurrentToken->ToString()).c_str()
                ));
                return Result;
            }

            // Go to the value of the token.
            Result->RegisterAdvance();
            Advance();

            auto ExprNode = Result->Register(Expression());
            if (Result->Error.IsValid()) {
                return Result;
            }

            Result->Success(VarAssignNode::Alloc<VarAssignNode, ParserNode>(1, VarName.Ptr(), ExprNode.Ptr()));
            return Result;
        }
    }

	auto NodeBinOp = Result->Register(BinaryOperation(
        [=] { return Term(); },
        { TokenType::Plus, TokenType::Minus }
    ));

    if (Result->Error.IsValid()) {
        Result->Failure(new InvalidSyntaxError(
            CurrentToken->GetStartPosition(),
            CurrentToken->GetEndPosition(),
            fmt::format("Expected var, int, float, identifier, '+', '-', or '(' got {0}", CurrentToken->ToString()).c_str()
        ));
        return Result;
    }

    Result->Success(NodeBinOp);
    return Result;
}
