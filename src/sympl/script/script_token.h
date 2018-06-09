/**********************************************************
 * Author:  GameSencha, LLC
 * The MIT License (MIT)
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *  DEALINGS IN THE SOFTWARE.
 *
 **********************************************************/
#pragma once

#include <sympl/core/sympl_pch.h>
#include <sympl/core/object.h>
#include <sympl/core/mutex.h>

#include <sympl/thirdparty/urho3d/container/Vector.h>

sympl_nsstart

class StringBuffer;

/// Types of tokens read by the parser.
enum class TokenType
{
    Empty,
    Operator,
    Identifier,
    Delimiter,
    SpecialChar
};

/// Token meta information.
struct TokenMeta
{
    /// The name of the symbol.
    char Name[5];
    /// The alternative name for the symbol (used for decoding strings).
    char AltName;
    /// The value of the symbol.
    char Value[5];
    /// The type of symbol.
    TokenType Type;

    TokenMeta() noexcept {
        AltName = '\0';
        Type = TokenType::Empty;
        memset(Name, 0, 5);
        memset(Value, 0, 5);
    }

    bool IsEmpty() const {
        return Type == TokenType::Empty;
    }
};

class SYMPL_API ScriptToken : public Object
{
    SYMPL_OBJECT(ScriptToken, Object);

private:
    static TokenMeta Empty;

    /// The map of standard tokens.
    std::vector<TokenMeta*> _StdTokens;
    /// The map of delimiter tokens.
    std::vector<TokenMeta*> _DelTokens;
    /// The map of special character tokens.
    std::vector<TokenMeta*> _SpecTokens;

    /// Reference to the translation buffer (special chars decode).
    StringBuffer* _TranslateBuffer = nullptr;
    /// Reference to the resulted translation buffer (special chars decode).
    StringBuffer* _ResultBuffer = nullptr;

    //! Adds a standard symbol to the helper.
    //! \param type
    //! \param name
    //! \param value
    void AddStdToken(TokenType type, const char* name, const char* value);

    //! Adds a delimiter symbol to the helper.
    //! \param type
    //! \param name
    //! \param value
    void AddDelToken(TokenType type, const char* name, const char* value);

    //! Adds a special character symbol to the helper.
    //! \param type
    //! \param name
    //! \param value
    void AddSpecialCharToken(TokenType type, const char* name, const char* value);

    //! Finds a token.
    //! \param type
    //! \param token
    //! \return
    TokenMeta* FindToken(TokenType type, const char* token);

    //! Finds a token.
    //! \param type
    //! \param token
    //! \return
    TokenMeta* FindToken(TokenType type, const char token);

public:
    //! Constructor.
    ScriptToken();

    //! Destructor.
    ~ScriptToken() override;

    //! Called in place of the constructor.
    void __Construct() override;

    //! Returns if the input is a specific type.
    //! \param type
    //! \param input
    //! \return
    const bool IsType(TokenType type, const char* input);

    //! Returns if the input is a specific type.
    //! \param type
    //! \param input
    //! \return
    const bool IsType(TokenType type, const char input);

    //! Returns whether or not the input is an object.
    //! \param input
    //! \return
    const bool IsObject(const char input);

    //! Returns whether or not the input is an operator type.
    //! \param input
    //! \return
    const bool IsOperator(const char input);

    //! Returns whether or not the input is an operator type.
    //! \param input
    //! \return
    const bool IsOperator(const char* input);

    //! Returns whether or not the input is an identifier.
    //! \param input
    //! \return
    const bool IsIdentifier(const char input);

    //! Returns whether or not the input is an identifier.
    //! \param input
    //! \return
    const bool IsIdentifier(const char* input);

    //! Returns whether or not the input is a delimiter.
    //! \param input
    //! \return
    const bool IsDelimiter(const char input);

    //! Returns whether or not the input is a special character.
    //! \param input
    //! \return
    const bool IsSpecialChar(const char input);

    //! Attempts to encode a given special character.
    //! \param input
    //! \param output
    //! \return
    bool EncodeSpecialChar(const char input, std::string& output);

    //! Returns a string where any special characters have been converted.
    //! \param input
    //! \return
    const std::string EncodeSpecialCharString(const char* input);

    //! Attempts to decode a given encoded character.
    //! \param input
    //! \param output
    //! \return
    bool DecodeSpecialChar(const char* input, char& output);

    //! Returns a string where any special characters have been converted.
    //! \param input
    //! \return
    const std::string DecodeSpecialCharString(const char* input);

    //! Releases the object.
    bool Release() override;
};

sympl_nsend
