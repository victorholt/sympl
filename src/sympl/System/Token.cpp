//
// GameSencha, LLC 5/24/22.
//
#include "Token.hpp"
#include <fmt/format.h>
SymplNamespace;

Token::Token(TokenType Type, CStrPtr Value) : Type(Type), Value(Value)
{
	// Zero out the tmp string.
	memset(TmpAlloc_ToString, 0, strlen(TmpAlloc_ToString));
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
	memset(TmpAlloc_ToString, 0, strlen(TmpAlloc_ToString));

	if (Value && strlen(Value) > 0) {
		memcpy(TmpAlloc_ToString, fmt::format("{0}:{1}", TypeStr, Value).c_str(), strlen(TmpAlloc_ToString));
		return TmpAlloc_ToString;
	}

	memcpy(TmpAlloc_ToString, fmt::format("{0}}", TypeStr).c_str(), strlen(TmpAlloc_ToString));
	return TmpAlloc_ToString;
}

Token& Token::operator=(const Token& rhs) {
	this->Type = rhs.Type;
	this->Value = rhs.Value;
	return *this;
}
