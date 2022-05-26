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

void Token::Copy(SharedPtr<Token> CopyToken)
{
    Type = CopyToken->Type;
    Value = CopyToken->Value;
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

	if (Value->Length() > 0) {
		strcpy(TmpAlloc_ToString, fmt::format("{0}:{1}", TypeStr, Value->CStr()).c_str());
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
