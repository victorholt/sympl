//
// GameSencha, LLC 5/24/22.
//
#include "Token.hpp"
#include "LexerPosition.hpp"
#include <sympl/Core/StringBuffer.hpp>
#include <fmt/format.h>
SymplNamespace;

void Token::__Construct(int argc, va_list ArgList)
{
    TokenType pType = va_arg(ArgList, TokenType);
    CStrPtr pValueStr = va_arg(ArgList, CStrPtr);
    SharedPtr<LexerPosition> pStartPosition = va_arg(ArgList, LexerPosition*);
    SharedPtr<LexerPosition> pEndPosition = va_arg(ArgList, LexerPosition*);

    Create(pType, pValueStr, pStartPosition, pEndPosition);
}

void Token::Create(TokenType pType, CStrPtr ValueStr, SharedPtr<LexerPosition> pStartPosition, SharedPtr<LexerPosition> pEndPosition)
{
    Type = pType;
    Value = StringBuffer::Alloc<StringBuffer>();
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
        Value->Set(ValueStr);
    }

	// Zero out the tmp string.
	memset(TmpAlloc_ToString, 0, sizeof(TmpAlloc_ToString));
}

void Token::CopyFrom(SharedPtr<Token> CopyToken)
{
    Type = CopyToken->Type;
    Value = CopyToken->Value;
}

SharedPtr<Token> Token::Copy()
{
    return Token::Alloc<Token>(4, Type, Value->CStr(), StartPosition.Ptr(), EndPosition.Ptr());
}

CStrPtr Token::GetValue() const
{
    if (!Value.IsValid()) {
        return "";
    }
    return Value->CStr();
}

CStrPtr Token::ToString()
{
	if (Type == TokenType::String) {
		return Value->CStr();
	}

	char TypeStr[10];
	memset(TypeStr, 0, 10);

	switch (Type)
	{
        case TokenType::EndOfFile:
            strcpy(TypeStr, TokenValue(TokenType::EndOfFile));
            break;
		case TokenType::Int:
			strcpy(TypeStr, TokenValue(TokenType::Int));
			break;
		case TokenType::Float:
			strcpy(TypeStr, TokenValue(TokenType::Float));
			break;
		case TokenType::Plus:
			strcpy(TypeStr, TokenValue(TokenType::Plus));
			break;
		case TokenType::Minus:
			strcpy(TypeStr, TokenValue(TokenType::Minus));
			break;
		case TokenType::Mul:
			strcpy(TypeStr, TokenValue(TokenType::Mul));
			break;
		case TokenType::Div:
			strcpy(TypeStr, TokenValue(TokenType::Div));
			break;
		case TokenType::LH_Parenth:
			strcpy(TypeStr, TokenValue(TokenType::LH_Parenth));
			break;
		case TokenType::RH_Parenth:
			strcpy(TypeStr, TokenValue(TokenType::RH_Parenth));
			break;
        case TokenType::L_SqrBracket:
            strcpy(TypeStr, TokenValue(TokenType::L_SqrBracket));
            break;
        case TokenType::R_SqrBracket:
            strcpy(TypeStr, TokenValue(TokenType::R_SqrBracket));
            break;
        case TokenType::Identifier:
            strcpy(TypeStr, TokenValue(TokenType::Identifier));
            break;
        case TokenType::Keyword:
            strcpy(TypeStr, TokenValue(TokenType::Keyword));
            break;
        case TokenType::Equals:
            strcpy(TypeStr, TokenValue(TokenType::Equals));
            break;
        case TokenType::IsEqual:
            strcpy(TypeStr, TokenValue(TokenType::IsEqual));
            break;
        case TokenType::NotEqual:
            strcpy(TypeStr, TokenValue(TokenType::NotEqual));
            break;
        case TokenType::LessThan:
            strcpy(TypeStr, TokenValue(TokenType::LessThan));
            break;
        case TokenType::LessThanOrEqual:
            strcpy(TypeStr, TokenValue(TokenType::LessThanOrEqual));
            break;
        case TokenType::GreaterThan:
            strcpy(TypeStr, TokenValue(TokenType::GreaterThan));
            break;
        case TokenType::GreaterThanOrEqual:
            strcpy(TypeStr, TokenValue(TokenType::GreaterThanOrEqual));
            break;
        case TokenType::Not:
            strcpy(TypeStr, TokenValue(TokenType::Not));
            break;
        case TokenType::Or:
            strcpy(TypeStr, TokenValue(TokenType::Or));
            break;
        case TokenType::And:
            strcpy(TypeStr, TokenValue(TokenType::And));
            break;
        case TokenType::Comma:
            strcpy(TypeStr, TokenValue(TokenType::Comma));
            break;
        case TokenType::Arrow:
            strcpy(TypeStr, TokenValue(TokenType::Arrow));
            break;
        case TokenType::NewLine:
            strcpy(TypeStr, TokenValue(TokenType::CloseStatement));
            break;
	}

	// Zero out the tmp string.
	memset(TmpAlloc_ToString, 0, sizeof(TmpAlloc_ToString));

	if (Value->Length() > 0) {
		strcpy(TmpAlloc_ToString, fmt::format("{0}: '{1}'", TypeStr, Value->CStr()).c_str());
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
