//
// Created by Victor on 5/24/2022.
//
#include "Parser.hpp"
#include "Token.hpp"
#include "ParserNode.hpp"
#include "ParserBinaryNode.hpp"
SymplNamespace

Parser::Parser(const std::vector<class Token>& pTokenList)
    : TokenIndex(0), CurrentToken(nullptr)
{
    TokenList = pTokenList;
    Advance();
}

ParserNodeObject Parser::Parse()
{
    return Expression();
}

Token* Parser::Advance()
{
    TokenIndex++;
    return GetCurrentToken();
}

ParserNodeObject Parser::Factor()
{
    switch (CurrentToken->GetType())
    {
        case TokenType::Int:
        case TokenType::Float:
            Advance();
            return { CurrentToken };

    }
}

ParserNodeObject Parser::Term()
{
    return BinaryOperation(
        [=] { return Factor(); },
        { TokenType::Mul, TokenType::Div }
    );
}

ParserNodeObject Parser::BinaryOperation(std::function<ParserNodeObject()> OpMethod, const std::vector<TokenType>& ValidOps)
{
    ParserNodeObject LeftNode = OpMethod();

    while (std::find(ValidOps.begin(), ValidOps.end(), CurrentToken->GetType()) != ValidOps.end())
    {
        Token* OpToken = CurrentToken;
        Advance();

        ParserNodeObject RightNode = OpMethod();
        LeftNode.BinaryNode = ParserBinaryNode(&LeftNode.BaseNode, OpToken, &RightNode.BaseNode);
    }

    return { LeftNode.BinaryNode };
}

ParserNodeObject Parser::Expression()
{
    return BinaryOperation(
            [=] { return Factor(); },
            { TokenType::Plus, TokenType::Minus }
    );
}

class Token* Parser::GetCurrentToken()
{
    if (TokenIndex >= TokenList.size()) {
        return nullptr;
    }

    CurrentToken = &TokenList[TokenIndex];

    return CurrentToken;
}

char* Parser::ToString() const
{

}
