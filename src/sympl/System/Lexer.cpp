//
// GameSencha, LLC 5/24/22.
//
#include "Lexer.hpp"
#include "Token.hpp"
#include "SyntaxError.hpp"
SymplNamespace

Lexer::Lexer(CStrPtr Text) : BufferPosition(0), CurrentChar('\0')
{
	Buffer = new char[strlen(Buffer) + 1];
	memset(Buffer, 0, strlen(Buffer));
	
	Digits = {'0','1','2','3','4','5','6','7','8','9'};
	Advance();
}

void Lexer::Advance()
{
	if (BufferPosition >= strlen(Buffer)) {
		CurrentChar = '\0';
		return;
	}

	CurrentChar = Buffer[BufferPosition++];
}

void Lexer::MakeTokens()
{
	CurrentChar = -1;
	while (CurrentChar != '\0') {
		Advance();

		// Ignore spaces and tabs.
		if (CurrentChar == ' ' || CurrentChar == '\t') {
			continue;
		}

		// Check if the character is a digit.
		if (std::find(Digits.begin(), Digits.end(), CurrentChar) != Digits.end()) {
			TokenList.emplace_back(MakeNumberToken());
			continue;
		}

		// Check for tokens.
		if (CurrentChar == '+') {
			TokenList.emplace_back(Token(TokenType::Plus, nullptr));
			Advance();
		}
		else if (CurrentChar == '-') {
			TokenList.emplace_back(Token(TokenType::Minus, nullptr));
			Advance();
		}
		else if (CurrentChar == '*') {
			TokenList.emplace_back(Token(TokenType::Mul, nullptr));
			Advance();
		}
		else if (CurrentChar == '/') {
			TokenList.emplace_back(Token(TokenType::Div, nullptr));
			Advance();
		}
		else if (CurrentChar == '(') {
			TokenList.emplace_back(Token(TokenType::LH_Parenth, nullptr));
			Advance();
		}
		else if (CurrentChar == ')') {
			TokenList.emplace_back(Token(TokenType::RH_Parenth, nullptr));
			Advance();
		}
		else {
			// Unable to find a proper token.
			Advance();
			TokenList.clear();
			ErrorList.emplace_back(SyntaxError(fmt::format("'{0}' on Line: {1}", CurrentChar, __LINE__).c_str()));
			return;
		}
	}
}

Token Lexer::MakeNumberToken()
{
	bool isFloat = false;
	char NumberStr[64];
	short NumberStrIndex = 0;
	memset(NumberStr, 0, 64);

	while (CurrentChar != '\0' &&
	( std::find(Digits.begin(), Digits.end(), CurrentChar) != Digits.end() ||
	( !isFloat && CurrentChar == '.') ) )
	{
		if (CurrentChar == '.') {
			isFloat = true;
		}
		NumberStr[NumberStrIndex++] = CurrentChar;
	}

	if (isFloat) {
		return {TokenType::Float, NumberStr};
	}

	return {TokenType::Int, NumberStr };
}
