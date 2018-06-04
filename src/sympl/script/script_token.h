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
#include <sympl/core/object_ref.h>
#include <sympl/core/mutex.h>

sympl_nsstart

class StringBuffer;

/// Types of tokens read by the parser.
enum class TokenType
{
    Keyword,
    Operator,
    Identifier,
    Delimiter,
    SpecialChar
};

/// Token meta information.
struct TokenMeta
{
    /// The name of the symbol.
    std::string Name;
    /// The alternative name for the symbol (used for decoding strings).
    char AltName;
    /// The value of the symbol.
    std::string Value;
    /// The type of symbol.
    TokenType Type;
};

class SYMPL_API ScriptToken : public ObjectRef
{
    SYMPL_OBJECT(ScriptToken, ObjectRef);

private:
    /// The map of standard tokens.
    std::unordered_map<std::string, TokenMeta> _StdTokens;
    /// The map of delimiter tokens.
    std::unordered_map<std::string, TokenMeta> _DelTokens;
    /// The map of special character tokens.
    std::unordered_map<std::string, TokenMeta> _SpecTokens;

    /// Reference to the translation buffer (special chars decode).
    StringBuffer* _TranslateBuffer = nullptr;
    /// Reference to the resulted translation buffer (special chars decode).
    StringBuffer* _ResultBuffer = nullptr;

    //! Adds a standard symbol to the helper.
    //! \param type
    //! \param name
    //! \param value
    void AddStdToken(TokenType type, const std::string& name, const std::string& value);

    //! Adds a delimiter symbol to the helper.
    //! \param type
    //! \param name
    //! \param value
    void AddDelToken(TokenType type, const std::string& name, const std::string& value);

    //! Adds a special character symbol to the helper.
    //! \param type
    //! \param name
    //! \param value
    void AddSpecialCharToken(TokenType type, const std::string& name, const std::string& value);

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
    const bool IsType(TokenType type, const std::string& input) const;

    //! Returns if the input is a specific type.
    //! \param type
    //! \param input
    //! \return
    const bool IsType(TokenType type, const char input) const;

    //! Returns whether or not the input is an object.
    //! \param input
    //! \return
    const bool IsObject(const char input) const;

    //! Returns whether or not the input is an operator type.
    //! \param input
    //! \return
    const bool IsOperator(const char input) const;

    //! Returns whether or not the input is an operator type.
    //! \param input
    //! \return
    const bool IsOperator(const std::string& input) const;

    //! Returns whether or not the input is an identifier.
    //! \param input
    //! \return
    const bool IsIdentifier(const char input) const;

    //! Returns whether or not the input is an identifier.
    //! \param input
    //! \return
    const bool IsIdentifier(const std::string& input) const;

    //! Returns whether or not the input is a keyword.
    //! \param input
    //! \return
    const bool IsKeyword(const std::string& input) const;

    //! Returns whether or not the input is a delimiter.
    //! \param input
    //! \return
    const bool IsDelimiter(const char input) const;

    //! Returns whether or not the input is a special character.
    //! \param input
    //! \return
    const bool IsSpecialChar(const char input) const;

    //! Attempts to encode a given special character.
    //! \param input
    //! \param output
    //! \return
    bool EncodeSpecialChar(const char input, std::string& output);

    //! Returns a string where any special characters have been converted.
    //! \param input
    //! \return
    const std::string EncodeSpecialCharString(const std::string& input);

    //! Attempts to decode a given encoded character.
    //! \param input
    //! \param output
    //! \return
    bool DecodeSpecialChar(const std::string& input, char& output);

    //! Returns a string where any special characters have been converted.
    //! \param input
    //! \return
    const std::string DecodeSpecialCharString(const std::string& input);

    //! Releases the object.
    bool Release() override;
};

sympl_nsend