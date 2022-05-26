//
// GameSencha, LLC 5/24/22.
//
#include "Token.hpp"
#include "LexerPosition.hpp"
#include <fmt/format.h>
SymplNamespace;

Token::Token(TokenType Type, CStrPtr ValueStr, SharedPtr<LexerPosition> pStartPosition, SharedPtr<LexerPosition> pEndPosition) : Type(Type)
{
    Value = nullptr;
    StartPosition = nullptr;
    EndPosition = nullptr;

    if (pStartPosition.IsValid()) {
        StartPosition = pStartPosition->Copy();
        EndPosition = pStartPosition->Copy();
        EndPosition->Advance();
    }

    if (pEndPosition.IsValid()) {
        EndPosition = pEndPosition;
    }

    if (ValueStr) {
        Value = std::make_shared<StrPtr>(new char[strlen(ValueStr) + 1]);
        strcpy(*Value, ValueStr);
    }

	// Zero out the tmp string.
	memset(TmpAlloc_ToString, 0, sizeof(TmpAlloc_ToString));
}

Token::Token(Token* CopyToken)
{
    this->Type = CopyToken->Type;
    this->Value = CopyToken->Value;
}

CStrPtr Token::ToString()
{
	char TypeStr[10];
	memset(TypeStr, 0, 10);

	switch (Type)
	{
		case TokenType::Int:
			strcpy(TypeStr, "int");
			break;
		case TokenType::Float:
			strcpy(TypeStr, "float");
			break;
		case TokenType::Plus:
			strcpy(TypeStr, "+");
			break;
		case TokenType::Minus:
			strcpy(TypeStr, "-");
			break;
		case TokenType::Mul:
			strcpy(TypeStr, "*");
			break;
		case TokenType::Div:
			strcpy(TypeStr, "/");
			break;
		case TokenType::LH_Parenth:
			strcpy(TypeStr, "(");
			break;
		case TokenType::RH_Parenth:
			strcpy(TypeStr, ")");
			break;
	}

	// Zero out the tmp string.
	memset(TmpAlloc_ToString, 0, sizeof(TmpAlloc_ToString));

	if (Value && strlen(*Value) > 0) {
		strcpy(TmpAlloc_ToString, fmt::format("{0}:{1}", TypeStr, *Value).c_str());
		return TmpAlloc_ToString;
	}

	strcpy(TmpAlloc_ToString, fmt::format("{0}", TypeStr).c_str());
	return TmpAlloc_ToString;
}

Token& Token::operator=(const Token& rhs) {
    if (this == &rhs) {
        return *this;
    }

	this->Type = rhs.Type;
    this->Value = rhs.Value;

	return *this;
}
