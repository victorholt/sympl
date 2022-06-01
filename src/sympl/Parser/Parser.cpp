//
// Created by Victor on 5/24/2022.
//
#include "Parser.hpp"
#include "Token.hpp"
#include "LexerPosition.hpp"
#include "sympl/Parser/Error/InvalidSyntaxError.hpp"
#include "sympl/Parser/Node/ParserIfNode.hpp"
#include "sympl/Parser/Node/ParserForNode.hpp"
#include "sympl/Parser/Node/ParserWhileNode.hpp"
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
                fmt::format("<parse> Expected '+', '-', '*' or '/', got {0}", CurrentToken->ToString()).c_str()
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
                        fmt::format("<atom> Expected ')', got {0}", FactorToken->ToString()).c_str()
                ));
                return Expr;
            }
        }

		// Keywords
		case TokenType::Keyword:
		{
			if (TokenIsKeyword(FactorToken, IfKeyword)) {
				auto IfExprNode = Result->Register(IfExpr());
				if (Result->Error.IsValid()) {
					return Result;
				}

				Result->Success(IfExprNode);
				return Result;
			}

			if (TokenIsKeyword(FactorToken, ForKeyword)) {
				auto ForExprNode = Result->Register(ForExpr());
				if (Result->Error.IsValid()) {
					return Result;
				}

				Result->Success(ForExprNode);
				return Result;
			}

			if (TokenIsKeyword(FactorToken, WhileKeyword)) {
				auto WhileExprNode = Result->Register(WhileExpr());
				if (Result->Error.IsValid()) {
					return Result;
				}

				Result->Success(WhileExprNode);
				return Result;
			}

			break;
		}
    }

    Result->Failure(new InvalidSyntaxError(
        FactorToken->GetStartPosition(),
        FactorToken->GetEndPosition(),
        fmt::format("<atom> Expected int, float, identifier, '+', '-', or '(' got {0}", FactorToken->ToString()).c_str()
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
        {
				{TokenType::Mul, TokenValue(TokenType::Mul)},
				{TokenType::Div, TokenValue(TokenType::Div)}
		}
    );
}

SharedPtr<ParseResult> Parser::BinaryOperation(
    const std::function<SharedPtr<ParseResult>()>& LeftOpMethod,
    const std::unordered_map<TokenType, std::string>& ValidOps,
    const std::function<SharedPtr<ParseResult>()>& RightOpMethod
)
{
	SharedPtr<ParseResult> Result = ParseResult::Alloc<ParseResult>();
	auto LeftNodeResult = LeftOpMethod();

    if (LeftNodeResult->Error.IsValid()) {
        return LeftNodeResult;
    }

	std::vector<TokenType> OpKeys;
	std::vector<std::string> OpValues;
	for(const auto& it : ValidOps) {
		OpKeys.push_back(it.first);
		OpValues.push_back(it.second);
	}

    while ( VectorHas(OpKeys, CurrentToken->GetType()) || VectorHas(OpValues, CurrentToken->GetValue()) )
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
        { {TokenType::Power, TokenValue(TokenType::Power)} },
        [=] { return Factor(); }
    );
}

SharedPtr<ParseResult> Parser::Expression()
{
    SharedPtr<ParseResult> Result = ParseResult::Alloc<ParseResult>();

    if (CurrentToken->GetType() == TokenType::Keyword)
    {
        if (strcmp(CurrentToken->GetValue(), VarKeyword) == 0) {
            Result->RegisterAdvance();
            Advance();

            // Advanced and didn't find a proper identifier.
            if (CurrentToken->GetType() != TokenType::Identifier)
            {
                Result->Failure(new InvalidSyntaxError(
                    CurrentToken->GetStartPosition(),
                    CurrentToken->GetEndPosition(),
                    fmt::format("<expr> Expected variable identifier, got {0}", CurrentToken->ToString()).c_str()
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
                    fmt::format("<expr> Expected = token, got {0}", CurrentToken->ToString()).c_str()
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
        [=] { return CompExpr(); },
        {
			{TokenType::And, TokenValue(TokenType::And)},
			{TokenType::Or, TokenValue(TokenType::Or)}
		}
    ));

    if (Result->Error.IsValid()) {
        Result->Failure(new InvalidSyntaxError(
            CurrentToken->GetStartPosition(),
            CurrentToken->GetEndPosition(),
            fmt::format("<expr> Expected var, int, float, identifier, '+', '-', or '(' got {0}", CurrentToken->ToString()).c_str()
        ));
        return Result;
    }

    Result->Success(NodeBinOp);
    return Result;
}

SharedPtr<ParseResult> Parser::CompExpr() {
	SharedPtr<ParseResult> Result = ParseResult::Alloc<ParseResult>();
	SharedPtr<ParserNode> Node;

	if (CurrentToken->GetType() == TokenType::Not)
	{
		Result->RegisterAdvance();
		Advance();

		Node = Result->Register(CompExpr());
		if (Result->Error.IsValid()) {
			Result->Failure(new InvalidSyntaxError(
				CurrentToken->GetStartPosition(),
				CurrentToken->GetEndPosition(),
				fmt::format("<comp_expr> Expected var, int, float, identifier, '+', '-', or '(' got {0}", CurrentToken->ToString()).c_str()
			));
			return Result;
		}

		Result->Success(
			ParserUnaryOpNode::Alloc<ParserUnaryOpNode, ParserNode>(2, CurrentToken, Node.Ptr())
		);
	}

	Node = Result->Register(BinaryOperation(
			[=] { return ArithExpr(); },
			{
				{TokenType::IsEqual, TokenValue(TokenType::IsEqual)},
				{TokenType::NotEqual, TokenValue(TokenType::NotEqual)},
				{TokenType::LessThan, TokenValue(TokenType::LessThan)},
				{TokenType::LessThanOrEqual, TokenValue(TokenType::LessThanOrEqual)},
				{TokenType::GreaterThan, TokenValue(TokenType::GreaterThan)},
				{TokenType::GreaterThanOrEqual, TokenValue(TokenType::GreaterThanOrEqual)}
			}
	));

	if (Result->Error.IsValid())
	{
		Result->Failure(new InvalidSyntaxError(
			CurrentToken->GetStartPosition(),
			CurrentToken->GetEndPosition(),
			fmt::format("<comp_expr> Expected int, float, identifier, '+', '-', '(', or '!' got {0}", CurrentToken->ToString()).c_str()
		));
		return Result;
	}

	Result->Success(Node);
	return Result;
}

SharedPtr<ParseResult> Parser::ArithExpr() {
	return BinaryOperation(
		[=] { return Term(); },
		{
			{TokenType::Plus, TokenValue(TokenType::Plus)},
			{TokenType::Minus, TokenValue(TokenType::Minus)}
		}
	);
}

SharedPtr<ParseResult> Parser::IfExpr() {
	SharedPtr<ParseResult> Result = ParseResult::Alloc<ParseResult>();
	IfCaseList Cases;
	SharedPtr<ParserNode> ElseCase;

	if (TokenIsNotKeyword(CurrentToken, IfKeyword)) {
		Result->Failure(new InvalidSyntaxError(
			CurrentToken->GetStartPosition(),
			CurrentToken->GetEndPosition(),
			fmt::format("<if_expr> Expected 'if' got {0}", CurrentToken->ToString()).c_str()
		));
		return Result;
	}

	Result->RegisterAdvance();
	Advance();

	// Grab the condition from the expression.
	SharedPtr<ParserNode> Condition = Result->Register(Expression());
	if (Result->Error.IsValid()) {
		return Result;
	}

	// Our next token should be the 'then' keyword.
	if (TokenIsNotKeyword(CurrentToken, ThenKeyword)) {
		Result->Failure(new InvalidSyntaxError(
			CurrentToken->GetStartPosition(),
			CurrentToken->GetEndPosition(),
			fmt::format("<if_expr> Expected 'then' got {0}", CurrentToken->ToString()).c_str()
		));
		return Result;
	}

	Result->RegisterAdvance();
	Advance();

	// Grab the expression for the if statement.
	SharedPtr<ParserNode> Expr = Result->Register(Expression());
	if (Result->Error.IsValid()) {
		return Result;
	}
	// Save the expression.
	Cases.emplace_back(std::make_tuple(Condition, Expr));

	while (TokenIsKeyword(CurrentToken, ElseIfKeyword)) {
		Result->RegisterAdvance();
		Advance();

		Condition = Result->Register(Expression());
		if (Result->Error.IsValid()) {
			return Result;
		}

		// Our next token should be the 'then' keyword.
		if (TokenIsNotKeyword(CurrentToken, ThenKeyword)) {
			Result->Failure(new InvalidSyntaxError(
				CurrentToken->GetStartPosition(),
				CurrentToken->GetEndPosition(),
				fmt::format("<if_expr> Expected 'then' got {0}", CurrentToken->ToString()).c_str()
			));
			return Result;
		}

		Result->RegisterAdvance();
		Advance();

		Expr = Result->Register(Expression());
		if (Result->Error.IsValid()) {
			return Result;
		}
		Cases.emplace_back(std::make_tuple(Condition, Expr));
	}

	// Check if we are ending with an else statement.
	if (strcmp(CurrentToken->GetValue(), ElseKeyword) == 0) {
		Result->RegisterAdvance();
		Advance();

		ElseCase = Result->Register(Expression());
		if (Result->Error.IsValid()) {
			return Result;
		}
	}

	auto ResultIfNode = ParserIfNode::Alloc<ParserIfNode>();

	// Explicity call create here since I don't believe C++ can handle tuples in va_list???
	ResultIfNode->Create(CurrentToken, Cases, ElseCase);
	Result->Success(ResultIfNode.Ptr());

	return Result;
}

SharedPtr<ParseResult> Parser::ForExpr()
{
	SharedPtr<ParseResult> Result = ParseResult::Alloc<ParseResult>();

	// Ensure we have a for keyword.
	if (TokenIsNotKeyword(CurrentToken, ForKeyword)) {
		Result->Failure(new InvalidSyntaxError(
			CurrentToken->GetStartPosition(),
			CurrentToken->GetEndPosition(),
			fmt::format("<for_expr> Expected 'for' got {0}", CurrentToken->ToString()).c_str()
		));
		return Result;
	}

	Result->RegisterAdvance();
	Advance();

	// Ensure we have an identifier.
	if (CurrentToken->GetType() != TokenType::Identifier) {
		Result->Failure(new InvalidSyntaxError(
			CurrentToken->GetStartPosition(),
			CurrentToken->GetEndPosition(),
			fmt::format("<for_expr> Expected 'identifier' got {0}", CurrentToken->ToString()).c_str()
		));
		return Result;
	}

	auto VarNameToken = CurrentToken;
	Result->RegisterAdvance();
	Advance();

	// Identifier should assign a value.
	if (CurrentToken->GetType() != TokenType::Equals) {
		Result->Failure(new InvalidSyntaxError(
			CurrentToken->GetStartPosition(),
			CurrentToken->GetEndPosition(),
			fmt::format("<for_expr> Expected '=' got {0}", CurrentToken->ToString()).c_str()
		));
		return Result;
	}

	Result->RegisterAdvance();
	Advance();

	// Grab the start value.
	auto StartValue = Result->Register(Expression());
	if (Result->Error.IsValid()) {
		return Result;
	}

	// Ensure we have a 'to' keyword to set our ending value.
	if (TokenIsNotKeyword(CurrentToken, ToKeyword)) {
		Result->Failure(new InvalidSyntaxError(
			CurrentToken->GetStartPosition(),
			CurrentToken->GetEndPosition(),
			fmt::format("<for_expr> Expected 'to' got {0}", CurrentToken->ToString()).c_str()
		));
		return Result;
	}

	Result->RegisterAdvance();
	Advance();

	// Grab the end value expression.
	auto EndValue = Result->Register(Expression());
	if (Result->Error.IsValid()) {
		return Result;
	}

	// Ensure we have a 'step' keyword to set our ending value.
	SharedPtr<ParserNode> StepValue;
	if (TokenIsKeyword(CurrentToken, StepKeyword)) {
		Result->RegisterAdvance();
		Advance();

		StepValue = Result->Register(Expression());
		if (Result->Error.IsValid()) {
			return Result;
		}
	}

	// Ensure we have a 'then' keyword to capture the body.
	if (TokenIsNotKeyword(CurrentToken, ThenKeyword)) {
		Result->Failure(new InvalidSyntaxError(
			CurrentToken->GetStartPosition(),
			CurrentToken->GetEndPosition(),
			fmt::format("<for_expr> Expected 'then' got {0}", CurrentToken->ToString()).c_str()
		));
		return Result;
	}

	Result->RegisterAdvance();
	Advance();

	// Grab the body value expression.
	auto BodyNode = Result->Register(Expression());
	if (Result->Error.IsValid()) {
		return Result;
	}

	Result->Success(ParserForNode::Alloc<ParserForNode>(
		5, VarNameToken, StartValue.Ptr(), EndValue.Ptr(), StepValue.IsValid() ? StepValue.Ptr() : nullptr, BodyNode.Ptr()
	).Ptr());

	return Result;
}

SharedPtr<ParseResult> Parser::WhileExpr()
{
	SharedPtr<ParseResult> Result = ParseResult::Alloc<ParseResult>();

	// Ensure we have a for keyword.
	if (TokenIsNotKeyword(CurrentToken, WhileKeyword)) {
		Result->Failure(new InvalidSyntaxError(
			CurrentToken->GetStartPosition(),
			CurrentToken->GetEndPosition(),
			fmt::format("<while_expr> Expected 'for' got {0}", CurrentToken->ToString()).c_str()
		));
		return Result;
	}

	Result->RegisterAdvance();
	Advance();

	// Grab the start value.
	auto Condition = Result->Register(Expression());
	if (Result->Error.IsValid()) {
		return Result;
	}

	// Ensure we have a 'to' keyword to set our ending value.
	if (TokenIsNotKeyword(CurrentToken, ThenKeyword)) {
		Result->Failure(new InvalidSyntaxError(
				CurrentToken->GetStartPosition(),
				CurrentToken->GetEndPosition(),
				fmt::format("<while_expr> Expected 'then' got {0}", CurrentToken->ToString()).c_str()
		));
		return Result;
	}

	Result->RegisterAdvance();
	Advance();

	// Grab the body value expression.
	auto BodyNode = Result->Register(Expression());
	if (Result->Error.IsValid()) {
		return Result;
	}

	Result->Success(ParserWhileNode::Alloc<ParserWhileNode>(
		3, CurrentToken, Condition.Ptr(), BodyNode.Ptr()
	).Ptr());

	return Result;
}
