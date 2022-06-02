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
#include "sympl/Parser/Node/ParserFuncDefNode.hpp"
#include "sympl/Parser/Node/ParserCallNode.hpp"
#include "sympl/Parser/Node/ParserListNode.hpp"
#include "sympl/Parser/Node/ParserStringNode.hpp"
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

SharedPtr<ParseResult> Parser::Power()
{
    return BinaryOperation(
        [=] { return CallFunc(); },
        { {TokenType::Power, TokenValue(TokenType::Power)} },
        [=] { return Factor(); }
    );
}

SharedPtr<ParseResult> Parser::CallFunc()
{
    SharedPtr<ParseResult> Result = ParseResult::Alloc<ParseResult>();
    auto AtomNode = Result->Register(Atom());
    if (Result->Error.IsValid()) {
        return Result;
    }

    // Check if we're calling the atom.
    if (CurrentToken->GetType() == TokenType::LH_Parenth)
    {
        Result->RegisterAdvance();
        Advance();

        std::vector<SharedPtr<ParserNode>> ArgList;
        if (CurrentToken->GetType() == TokenType::RH_Parenth)
        {
            Result->RegisterAdvance();
            Advance();
        } else {
            ArgList.emplace_back(Result->Register(Expression()));
            if (Result->Error.IsValid()) {
                Result->Failure(new InvalidSyntaxError(
                    CurrentToken->GetStartPosition(),
                    CurrentToken->GetEndPosition(),
                    fmt::format("<atom> Expected ')', 'var', 'if', 'for', 'while', 'func', int, float, identifier, got {0}", CurrentToken->ToString()).c_str()
                ));
                return Result;
            }

            // Check for more arguments.
            while (CurrentToken->GetType() == TokenType::Comma)
            {
                Result->RegisterAdvance();
                Advance();

                ArgList.emplace_back(Result->Register(Expression()));
                if (Result->Error.IsValid()) {
                    return Result;
                }
            }

            // Check for closing parenth.
            if (CurrentToken->GetType() != TokenType::RH_Parenth) {
                Result->Failure(new InvalidSyntaxError(
                    CurrentToken->GetStartPosition(),
                    CurrentToken->GetEndPosition(),
                    fmt::format("<atom> Expected ',' or ')', got {0}", CurrentToken->ToString()).c_str()
                ));
                return Result;
            }

            Result->RegisterAdvance();
            Advance();
        }

        auto CallNode = ParserCallNode::Alloc<ParserCallNode>();
        CallNode->Create(CurrentToken, AtomNode, ArgList);
        Result->Success(CallNode.Ptr());
        return Result;
    }

    Result->Success(AtomNode);
    return Result;
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

        case TokenType::String: {
            Result->RegisterAdvance();
            Advance();
            Result->Success(ParserStringNode::Alloc<ParserStringNode, ParserNode>(1, FactorToken.Ptr()));
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

        case TokenType::L_SqrBracket:
        {
            auto ListExprResult = Result->Register(ListExpr());
            if (Result->Error.IsValid()) {
                return Result;
            }

            Result->Success(ListExprResult);
            return Result;
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

            if (TokenIsKeyword(FactorToken, FuncKeyword)) {
                auto FuncDefNode = Result->Register(FuncDef());
                if (Result->Error.IsValid()) {
                    return Result;
                }

                Result->Success(FuncDefNode);
                return Result;
            }

			break;
		}
    }

    Result->Failure(new InvalidSyntaxError(
        FactorToken->GetStartPosition(),
        FactorToken->GetEndPosition(),
        fmt::format("<atom> Expected int, float, identifier, '+', '-', '(', '[', 'if', 'for', 'while', or 'func' got {0}", FactorToken->ToString()).c_str()
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

SharedPtr<ParseResult> Parser::Expression()
{
    SharedPtr<ParseResult> Result = ParseResult::Alloc<ParseResult>();

    if (TokenIsKeyword(CurrentToken, VarKeyword)) {
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
            fmt::format("<expr> Expected var, int, float, identifier, '+', '-', '(', '[', '!', 'var', 'if', 'for', 'while', or 'func' got {0}", CurrentToken->ToString()).c_str()
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
				fmt::format("<comp_expr> Expected var, int, float, identifier, '+', '-', '(', or '[', got {0}", CurrentToken->ToString()).c_str()
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
			fmt::format("<comp_expr> Expected int, float, identifier, '+', '-', '(', '[', or '!' got {0}", CurrentToken->ToString()).c_str()
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

SharedPtr<ParseResult> Parser::ListExpr()
{
    SharedPtr<ParseResult> Result = ParseResult::Alloc<ParseResult>();
    std::vector<SharedPtr<ParserNode>> ElementNodes;
    auto StartPosition = CurrentToken->GetStartPosition()->Copy();

    // Opening square bracket.
    if (CurrentToken->GetType() != TokenType::L_SqrBracket)
    {
        Result->Failure(new InvalidSyntaxError(
		CurrentToken->GetStartPosition(),
		CurrentToken->GetEndPosition(),
		fmt::format("<list_expr> Expected int, float, identifier, '+', '-', '(', '[', or '!' got {0}", CurrentToken->ToString()).c_str()
        ));
    }

    Result->RegisterAdvance();
    Advance();

    // Check if we've closed the bracket (empty list).
    if (CurrentToken->GetType() == TokenType::R_SqrBracket)
    {
        Result->RegisterAdvance();
        Advance();
    }
    else {
        // Check for element.
        ElementNodes.emplace_back(Result->Register(Expression()));
        if (Result->Error.IsValid()) {
            Result->Failure(new InvalidSyntaxError(
				CurrentToken->GetStartPosition(),
				CurrentToken->GetEndPosition(),
				fmt::format("<list_expr> Expected ']', '[', '(', '+', '-', 'var', 'if', 'for', 'while', 'func', int, float, identifier, got {0}", CurrentToken->ToString()).c_str()
            ));
            return Result;
        }

        // Check for more arguments.
        while (CurrentToken->GetType() == TokenType::Comma)
        {
            Result->RegisterAdvance();
            Advance();

            ElementNodes.emplace_back(Result->Register(Expression()));
            if (Result->Error.IsValid()) {
                return Result;
            }
        }

        // Check for closing bracket.
        if (CurrentToken->GetType() != TokenType::R_SqrBracket) {
            Result->Failure(new InvalidSyntaxError(
                CurrentToken->GetStartPosition(),
                CurrentToken->GetEndPosition(),
                fmt::format("<list_expr> Expected ',' or ']', got {0}", CurrentToken->ToString()).c_str()
            ));
            return Result;
        }

        Result->RegisterAdvance();
        Advance();
    }

    auto NewListNode = ParserListNode::Alloc<ParserListNode>();
    NewListNode->Create(CurrentToken, ElementNodes, StartPosition, CurrentToken->GetEndPosition()->Copy());
    Result->Success(NewListNode.Ptr());

    return Result;
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

	// Ensure we have a 'then' keyword to set our ending value.
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

SharedPtr<ParseResult> Parser::FuncDef()
{
    SharedPtr<ParseResult> Result = ParseResult::Alloc<ParseResult>();

    // Keyword 'func' check.
    if (TokenIsNotKeyword(CurrentToken, FuncKeyword)) {
        Result->Failure(new InvalidSyntaxError(
            CurrentToken->GetStartPosition(),
            CurrentToken->GetEndPosition(),
            fmt::format("<func_def> Expected 'func' got {0}", CurrentToken->ToString()).c_str()
        ));
        return Result;
    }

    Result->RegisterAdvance();
    Advance();

    // Check if we have a name for the function (optional).
    SharedPtr<Token> FuncNameToken;
    if (CurrentToken->GetType() == TokenType::Identifier)
    {
        FuncNameToken = CurrentToken;
        Result->RegisterAdvance();
        Advance();

        // Ensure we have a left parenth.
        if (CurrentToken->GetType() != TokenType::LH_Parenth) {
            Result->Failure(new InvalidSyntaxError(
                CurrentToken->GetStartPosition(),
                CurrentToken->GetEndPosition(),
                fmt::format("<func_def> Expected '(' got {0}", CurrentToken->ToString()).c_str()
            ));
            return Result;
        }
    }
    else {
        // Ensure we have a left parenth.
        if (CurrentToken->GetType() != TokenType::LH_Parenth) {
            Result->Failure(new InvalidSyntaxError(
                CurrentToken->GetStartPosition(),
                CurrentToken->GetEndPosition(),
                fmt::format("<func_def> Expected '(' got {0}", CurrentToken->ToString()).c_str()
            ));
            return Result;
        }
    }

    Result->RegisterAdvance();
    Advance();

    // Check for arguments.
    std::vector<SharedPtr<Token>> ArgNameTokenList;

    if (CurrentToken->GetType() == TokenType::Identifier) {
        ArgNameTokenList.emplace_back(CurrentToken);
        Result->RegisterAdvance();
        Advance();

        // Check for more arguments.
        while (CurrentToken->GetType() == TokenType::Comma) {
            Result->RegisterAdvance();
            Advance();

            // Find another identifier.
            if (CurrentToken->GetType() != TokenType::Identifier) {
                Result->Failure(new InvalidSyntaxError(
                    CurrentToken->GetStartPosition(),
                    CurrentToken->GetEndPosition(),
                    fmt::format("<func_def> Expected 'identifier' got {0}", CurrentToken->ToString()).c_str()
                ));
                return Result;
            }

            ArgNameTokenList.emplace_back(CurrentToken);
            Result->RegisterAdvance();
            Advance();
        }

        // Check for the closing parenth.
        if (CurrentToken->GetType() != TokenType::RH_Parenth) {
            Result->Failure(new InvalidSyntaxError(
                CurrentToken->GetStartPosition(),
                CurrentToken->GetEndPosition(),
                fmt::format("<func_def> Expected ',' or ')' got {0}", CurrentToken->ToString()).c_str()
            ));
            return Result;
        }
    }
    else {
        // Check for the closing parenth.
        if (CurrentToken->GetType() != TokenType::RH_Parenth) {
            Result->Failure(new InvalidSyntaxError(
                CurrentToken->GetStartPosition(),
                CurrentToken->GetEndPosition(),
                fmt::format("<func_def> Expected 'identifier' or ')' got {0}", CurrentToken->ToString()).c_str()
            ));
            return Result;
        }
    }

    Result->RegisterAdvance();
    Advance();

    // Check for the body start function token.
    if (CurrentToken->GetType() != TokenType::Arrow) {
        Result->Failure(new InvalidSyntaxError(
                CurrentToken->GetStartPosition(),
                CurrentToken->GetEndPosition(),
                fmt::format("<func_def> Expected '->' got {0}", CurrentToken->ToString()).c_str()
        ));
        return Result;
    }

    Result->RegisterAdvance();
    Advance();

    auto NodeToReturn = Result->Register(Expression());
    if (Result->Error.IsValid()) {
        return Result;
    }

    // Create our new func def node.
    auto FuncDefNode = ParserFuncDefNode::Alloc<ParserFuncDefNode>();
    FuncDefNode->Create(FuncNameToken, ArgNameTokenList, NodeToReturn);

    Result->Success(FuncDefNode.Ptr());
    return Result;
}
