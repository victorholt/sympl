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
		}

        // Check if the character is a digit.
		else if (std::find(Digits.begin(), Digits.end(), CurrentChar) != Digits.end()) {
			TokenList.emplace_back(MakeNumberToken());
		}

        // Check for identifiers.
        else if (std::find(Letters.begin(), Letters.end(), CurrentChar) != Letters.end()) {
            TokenList.emplace_back(MakeIdentifier());
        }

        // Check for strings.
        else if (CurrentChar == TokenValueChar(TokenType::String)) {
            TokenList.emplace_back(MakeString());
        }

		// Check for tokens.
		else if (CurrentChar == TokenValueChar(TokenType::Plus)) {
			TokenList.emplace_back(Token::Alloc<Token>(4, TokenType::Plus, nullptr, Position.Ptr(), nullptr));
			Advance();
		}
		else if (CurrentChar == TokenValueChar(TokenType::Minus)) {
            TokenList.emplace_back(MakeMinusOrArrow());
		}
		else if (CurrentChar == TokenValueChar(TokenType::Mul)) {
			TokenList.emplace_back(Token::Alloc<Token>(4, TokenType::Mul, nullptr, Position.Ptr(), nullptr));
			Advance();
		}
		else if (CurrentChar == TokenValueChar(TokenType::Div)) {
			TokenList.emplace_back(Token::Alloc<Token>(4, TokenType::Div, nullptr, Position.Ptr(), nullptr));
			Advance();
		}
        else if (CurrentChar == TokenValueChar(TokenType::Power)) {
            TokenList.emplace_back(Token::Alloc<Token>(4, TokenType::Power, nullptr, Position.Ptr(), nullptr));
            Advance();
        }
		else if (CurrentChar == TokenValueChar(TokenType::LH_Parenth)) {
			TokenList.emplace_back(Token::Alloc<Token>(4, TokenType::LH_Parenth, nullptr, Position.Ptr(), nullptr));
			Advance();
		}
		else if (CurrentChar == TokenValueChar(TokenType::RH_Parenth)) {
			TokenList.emplace_back(Token::Alloc<Token>(4, TokenType::RH_Parenth, nullptr, Position.Ptr(), nullptr));
			Advance();
		}
        else if (CurrentChar == TokenValueChar(TokenType::Not)) {
            auto Result = MakeNotEquals();
            if (Result.IsValid()) {
                TokenList.emplace_back(Result);
            } else {
                TokenList.clear();
                ErrorList.emplace_back(CreateExpectedCharacterError("=", "!"));
                return;
            }
        }
        else if (CurrentChar == TokenValueChar(TokenType::Equals)) {
            TokenList.emplace_back(MakeEquals());
        }
        else if (CurrentChar == TokenValueChar(TokenType::GreaterThan)) {
            TokenList.emplace_back(MakeGreaterThan());
        }
        else if (CurrentChar == TokenValueChar(TokenType::LessThan)) {
            TokenList.emplace_back(MakeLessThan());
        }
		else if (CurrentChar == TokenValueChar(TokenType::Or)) {
			TokenList.emplace_back(MakeOrOp());
		}
		else if (CurrentChar == TokenValueChar(TokenType::And)) {
			TokenList.emplace_back(MakeAndOp());
		}
        else if (CurrentChar == TokenValueChar(TokenType::Comma)) {
            TokenList.emplace_back(Token::Alloc<Token>(4, TokenType::Comma, nullptr, Position.Ptr(), nullptr));
            Advance();
        }
		else {
			// Unable to find a proper token.
			TokenList.clear();
			ErrorList.emplace_back(CreateIllegalCharacterError());
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

SharedPtr<class Token> Lexer::MakeString()
{
    SharedPtr<StringBuffer> StrValue = StringBuffer::Alloc<StringBuffer>();
    auto StartPosition = Position->Copy();
    bool EscapeChar = false;
    Advance();

    std::unordered_map<char, char> EscapeCharList = {
        {'n', '\n'}, {'t', '\t'}, {'r', '\r'}
    };

    while (CurrentChar != '\0' && (CurrentChar != TokenValueChar(TokenType::String) || EscapeChar)) {
        if (EscapeChar) {
            StrValue->AppendByte(EscapeCharList.find(CurrentChar) != EscapeCharList.end() ? EscapeCharList[CurrentChar] : CurrentChar);
            Advance();
            EscapeChar = false;
        } else {
            if (CurrentChar == '\\') {
                EscapeChar = true;
            } else {
                StrValue->AppendByte(CurrentChar);
                Advance();
            }
        }
    }

    Advance();
    return Token::Alloc<Token>(4, TokenType::String, StrValue->CStr(), StartPosition.Ptr(), Position.Ptr());
}

SharedPtr<class Token> Lexer::MakeMinusOrArrow()
{
    TokenType Type = TokenType::Minus;
    auto StartPosition = Position->Copy();
    Advance();

    if (CurrentChar == '>') {
        Advance();
        Type = TokenType::Arrow;
    }

    return Token::Alloc<Token>(4, Type, nullptr, StartPosition.Ptr(), Position.Ptr());
}

SharedPtr<Token> Lexer::MakeNotEquals()
{
    auto StartPosition = Position->Copy();
    Advance();

    if (CurrentChar == '=') {
        Advance();
        return Token::Alloc<Token>(4, TokenType::NotEqual, TokenValue(TokenType::NotEqual), StartPosition.Ptr(), Position.Ptr());
    }

	Advance();
    return nullptr;
}

SharedPtr<Token> Lexer::MakeEquals()
{
	auto Type = TokenType::Equals;
    auto StartPosition = Position->Copy();
    Advance();

    if (CurrentChar == '=') {
        Advance();
		Type = TokenType::IsEqual;
    }

	return Token::Alloc<Token>(4, Type, TokenValue(Type), StartPosition.Ptr(), Position.Ptr());
}

SharedPtr<Token> Lexer::MakeGreaterThan()
{
	auto Type = TokenType::GreaterThan;
	auto StartPosition = Position->Copy();
	Advance();

	if (CurrentChar == '=') {
		Advance();
		Type = TokenType::GreaterThanOrEqual;
	}

	return Token::Alloc<Token>(4, Type, TokenValue(Type), StartPosition.Ptr(), Position.Ptr());
}

SharedPtr<Token> Lexer::MakeLessThan()
{
	auto Type = TokenType::LessThan;
	auto StartPosition = Position->Copy();
	Advance();

	if (CurrentChar == '=') {
		Advance();
		Type = TokenType::LessThanOrEqual;
	}

	return Token::Alloc<Token>(4, Type, TokenValue(Type), StartPosition.Ptr(), Position.Ptr());
}

SharedPtr<class Token> Lexer::MakeOrOp()
{
	auto StartPosition = Position->Copy();
	Advance();

	if (CurrentChar == '|') {
		Advance();
		return Token::Alloc<Token>(4, TokenType::Or, TokenValue(TokenType::Or), StartPosition.Ptr(), Position.Ptr());
	}

	Advance();
	return nullptr;
}

SharedPtr<class Token> Lexer::MakeAndOp()
{
	auto StartPosition = Position->Copy();
	Advance();

	if (CurrentChar == '&') {
		Advance();
		return Token::Alloc<Token>(4, TokenType::And, TokenValue(TokenType::And), StartPosition.Ptr(), Position.Ptr());
	}

	Advance();
	return nullptr;
}

SharedPtr<ParserError> Lexer::CreateIllegalCharacterError()
{
    return new IllegalCharacterError(
        fmt::format(
            "'{0}' on Line: {1} (Col: {2}) > {3}",
            CurrentChar,
            Position->GetLineNumber(),
            Position->GetLineCol(),
            Position->GetFileName()
        ).c_str()
    );
}

SharedPtr<ParserError> Lexer::CreateExpectedCharacterError(CStrPtr ExpectedCharacter, CStrPtr AfterCharacter)
{
    return new IllegalCharacterError(
        fmt::format(
            "'{0}' (after '{1}') on Line: {1} (Col: {2}) > {3}",
            ExpectedCharacter,
            AfterCharacter,
            Position->GetLineNumber(),
            Position->GetLineCol(),
            Position->GetFileName()
        ).c_str()
    );
}
