//
// GameSencha, LLC 5/24/22.
//
#include "Lexer.hpp"
#include "LexerPosition.hpp"
#include "Token.hpp"
#include <sympl/Core/StringBuffer.hpp>
#include <sympl/Parser/Error/IllegalCharacterError.hpp>
SymplNamespace

Lexer::Lexer(CStrPtr FileName, CStrPtr Text) :
    CurrentChar('\0')
{
    Position = LexerPosition::Alloc<LexerPosition>();
    Position->Create(0, 1, 0, FileName, Text);

    Buffer = StringBuffer::Alloc<StringBuffer>();
    Buffer->Set(Text);

	Digits = {'0','1','2','3','4','5','6','7','8','9'};
	Letters = {'a','b','c','d','e','f','g','h','i','j',
               'k','l','m','n','o','p','q','r','s','t','u',
               'v','w','x','y','z',
               'A','B','C','D','E','F','G','H','I','J',
               'K','L','M','N','O','P','Q','R','S','T','U',
               'V','W','X','Y','Z',
               '0','1','2','3','4','5','6','7','8','9',
               '_'};
}

Lexer::~Lexer()
{
}

void Lexer::Advance()
{
    const size_t index = Position->GetIndex();

	if (index >= Buffer->Length()) {
		CurrentChar = '\0';
		return;
	}

	CurrentChar = Buffer->Get(index);
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

        if (std::find(Letters.begin(), Letters.end(), CurrentChar) != Letters.end()) {
            TokenList.emplace_back(MakeIdentifier());
            continue;
        }

		// Check for tokens.
		if (CurrentChar == '+') {
			TokenList.emplace_back(Token::Alloc<Token>(4, TokenType::Plus, nullptr, Position.Ptr(), nullptr));
			Advance();
		}
		else if (CurrentChar == '-') {
			TokenList.emplace_back(Token::Alloc<Token>(4, TokenType::Minus, nullptr, Position.Ptr(), nullptr));
			Advance();
		}
		else if (CurrentChar == '*') {
			TokenList.emplace_back(Token::Alloc<Token>(4, TokenType::Mul, nullptr, Position.Ptr(), nullptr));
			Advance();
		}
		else if (CurrentChar == '/') {
			TokenList.emplace_back(Token::Alloc<Token>(4, TokenType::Div, nullptr, Position.Ptr(), nullptr));
			Advance();
		}
        else if (CurrentChar == '^') {
            TokenList.emplace_back(Token::Alloc<Token>(4, TokenType::Power, nullptr, Position.Ptr(), nullptr));
            Advance();
        }
        else if (CurrentChar == '=') {
            TokenList.emplace_back(Token::Alloc<Token>(4, TokenType::Equals, nullptr, Position.Ptr(), nullptr));
            Advance();
        }
		else if (CurrentChar == '(') {
			TokenList.emplace_back(Token::Alloc<Token>(4, TokenType::LH_Parenth, nullptr, Position.Ptr(), nullptr));
			Advance();
		}
		else if (CurrentChar == ')') {
			TokenList.emplace_back(Token::Alloc<Token>(4, TokenType::RH_Parenth, nullptr, Position.Ptr(), nullptr));
			Advance();
		}
		else {
			// Unable to find a proper token.
			TokenList.clear();
			ErrorList.emplace_back(new IllegalCharacterError(
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

    TokenList.emplace_back(Token::Alloc<Token>(4, TokenType::EndOfFile, nullptr, Position.Ptr(), nullptr));
}

SharedPtr<Token> Lexer::MakeNumberToken()
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
		return Token::Alloc<Token>(4, TokenType::Float, NumberStr, StartPosition.Ptr(), Position.Ptr());
	}

	return Token::Alloc<Token>(4, TokenType::Int, NumberStr, StartPosition.Ptr(), Position.Ptr());
}

SharedPtr<class Token> Lexer::MakeIdentifier()
{
    auto IdentifierStr = StringBuffer::Alloc<StringBuffer>();
    auto StartPosition = Position->Copy();

    while (CurrentChar != '\0' &&
        ( std::find(Letters.begin(), Letters.end(), CurrentChar) != Letters.end()) )
    {
        IdentifierStr->AppendByte(CurrentChar);
        Advance();
    }

    auto Result = Token::Alloc<Token>(
        4,
        IdentifierStr->IsWithin(BuiltInKeywordList) ? TokenType::Keyword : TokenType::Identifier,
        IdentifierStr->CStr(),
        StartPosition.Ptr(),
        Position.Ptr()
    );

    return Result;
}
