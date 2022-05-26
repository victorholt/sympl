//
// GameSencha, LLC 5/24/22.
//
#include "Lexer.hpp"
#include "LexerPosition.hpp"
#include "Token.hpp"
#include "sympl/Parser/Error/IllegalCharacterError.hpp"
SymplNamespace

Lexer::Lexer(CStrPtr FileName, CStrPtr Text) :
    Position(SharedPtr<LexerPosition>(new LexerPosition(0, 1, 0, FileName, Text))),
    CurrentChar('\0')
{
	Buffer = new char[strlen(Text) + 1];
	memset(Buffer, 0, strlen(Text) + 1);
    strcpy(Buffer, Text);

	Digits = {'0','1','2','3','4','5','6','7','8','9'};
}

Lexer::~Lexer()
{
    delete [] Buffer;
}

void Lexer::Advance()
{
    const size_t index = Position->GetIndex();

	if (index >= strlen(Buffer)) {
		CurrentChar = '\0';
		return;
	}

	CurrentChar = Buffer[index];
    Position->Advance(CurrentChar);
}

void Lexer::MakeTokens()
{
	Advance();

	while (CurrentChar != '\0') {
		// Ignore spaces and tabs.
		if (CurrentChar == ' ' || CurrentChar == '\t') {
			Advance();
			continue;
		}

		// Check if the character is a digit.
		if (std::find(Digits.begin(), Digits.end(), CurrentChar) != Digits.end()) {
			TokenList.emplace_back(MakeNumberToken());
			continue;
		}

		// Check for tokens.
		if (CurrentChar == '+') {
			TokenList.emplace_back(Token(TokenType::Plus, nullptr, Position));
			Advance();
		}
		else if (CurrentChar == '-') {
			TokenList.emplace_back(Token(TokenType::Minus, nullptr, Position));
			Advance();
		}
		else if (CurrentChar == '*') {
			TokenList.emplace_back(Token(TokenType::Mul, nullptr, Position));
			Advance();
		}
		else if (CurrentChar == '/') {
			TokenList.emplace_back(Token(TokenType::Div, nullptr, Position));
			Advance();
		}
		else if (CurrentChar == '(') {
			TokenList.emplace_back(Token(TokenType::LH_Parenth, nullptr, Position));
			Advance();
		}
		else if (CurrentChar == ')') {
			TokenList.emplace_back(Token(TokenType::RH_Parenth, nullptr, Position));
			Advance();
		}
		else {
			// Unable to find a proper token.
			TokenList.clear();
			ErrorList.emplace_back(IllegalCharacterError(
                fmt::format(
                    "'{0}' on Line: {1} (Col: {2}) > {3}",
                    CurrentChar,
                    Position->GetLineNumber(),
                    Position->GetLineCol(),
                    Position->GetFileName()
                ).c_str()
            ));
			return;
		}
	}

    TokenList.emplace_back(Token(TokenType::EndOfFile, nullptr, Position));
}

Token Lexer::MakeNumberToken()
{
	bool isFloat = false;
	char NumberStr[64];
	short NumberStrIndex = 0;
	memset(NumberStr, 0, 64);

    SharedPtr<LexerPosition> StartPosition = Position->Copy();

	while (CurrentChar != '\0' &&
	( std::find(Digits.begin(), Digits.end(), CurrentChar) != Digits.end() ||
	( !isFloat && CurrentChar == '.') ) )
	{
		if (CurrentChar == '.') {
			isFloat = true;
		}
		NumberStr[NumberStrIndex++] = CurrentChar;
        Advance();
	}

	if (isFloat) {
		return {TokenType::Float, NumberStr, StartPosition, Position};
	}

	return {TokenType::Int, NumberStr, StartPosition, Position};
}
