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
#include <sympl/script/script_token.h>
#include <sympl/core/string_buffer.h>
sympl_namespaces

ScriptToken::ScriptToken()
{
    __Construct();
}

ScriptToken::~ScriptToken()
{
    Release();
}

void ScriptToken::__Construct()
{
    // Setup the tokens.
    AddStdToken(TokenType::Operator, "=", "=");
    AddStdToken(TokenType::Operator, "+", "+");
    AddStdToken(TokenType::Operator, "-", "-");
    AddStdToken(TokenType::Operator, "/", "/");
    AddStdToken(TokenType::Operator, "*", "*");
    AddStdToken(TokenType::Operator, ">", ">");
    AddStdToken(TokenType::Operator, "<", "<");
    AddStdToken(TokenType::Operator, ">=", ">=");
    AddStdToken(TokenType::Operator, "<=", "<=");
    AddStdToken(TokenType::Operator, "|", "|");
    AddStdToken(TokenType::Operator, "&", "&");
    AddStdToken(TokenType::Operator, "!", "!");

    AddStdToken(TokenType::Operator, "||", "||");
    AddStdToken(TokenType::Operator, "&&", "&&");
    AddStdToken(TokenType::Operator, "!=", "!=");
    AddStdToken(TokenType::Operator, "==", "==");

    AddStdToken(TokenType::Identifier, "{", "{");
    AddStdToken(TokenType::Identifier, "}", "}");
    AddStdToken(TokenType::Identifier, "[", "[");
    AddStdToken(TokenType::Identifier, "]", "]");

    AddDelToken(TokenType::Delimiter, ",", ",");
    AddDelToken(TokenType::Delimiter, ";", ";");

    AddSpecialCharToken(TokenType::SpecialChar, ",", "#c#");
    AddSpecialCharToken(TokenType::SpecialChar, ";", "#sc#");
    AddSpecialCharToken(TokenType::SpecialChar, "\'", "#q#");
    AddSpecialCharToken(TokenType::SpecialChar, "\"", "\"");

    _TranslateBuffer = alloc_ref(StringBuffer);
    _ResultBuffer = alloc_ref(StringBuffer);
}

const bool ScriptToken::IsType(TokenType type, const std::string& input) const
{
    auto tokenValue = _StdTokens.find(input);
    if (type != TokenType::Delimiter && tokenValue == _StdTokens.end()) {
        return false;
    } else if (type == TokenType::Delimiter) {
        tokenValue = _DelTokens.find(input);
        if (tokenValue == _DelTokens.end()) {
            return false;
        }
    }

    return (tokenValue->second->Type == type);
}

const bool ScriptToken::IsType(TokenType type, const char input) const
{
    std::string inputStr = "";
    inputStr += input;

    if (type == TokenType::SpecialChar) {
        return (_SpecTokens.find(inputStr) != _SpecTokens.end());
    }

    return IsType(type, inputStr);
}

const bool ScriptToken::IsObject(const char input) const
{
    std::locale loc;
    if (std::isspace(input, loc) || input == '\0') {
        return false;
    }
    if (IsOperator(input)) {
        return false;
    }
    if (IsIdentifier(input)) {
        return false;
    }

    return !IsDelimiter(input);
}

const bool ScriptToken::IsOperator(const char input) const
{
    return IsOperator(std::string(1, input));
}

const bool ScriptToken::IsOperator(const std::string& input) const
{
    return IsType(TokenType::Operator, input);
}

const bool ScriptToken::IsIdentifier(const char input) const
{
    return IsIdentifier(std::string(1, input));
}

const bool ScriptToken::IsIdentifier(const std::string& input) const
{
    return IsType(TokenType::Identifier, input);
}

const bool ScriptToken::IsKeyword(const std::string& input) const
{
    return IsType(TokenType::Keyword, input);
}

const bool ScriptToken::IsDelimiter(const char input) const
{
    return IsType(TokenType::Delimiter, input);
}

const bool ScriptToken::IsSpecialChar(const char input) const
{
    return IsType(TokenType::SpecialChar, input);
}

bool ScriptToken::EncodeSpecialChar(const char input, std::string& output)
{
    std::string inputStr = "";
    inputStr += input;

    TokenMeta* sData;
    auto tokenIt = _SpecTokens.find(inputStr);
    if (tokenIt == _SpecTokens.end()) {
        return false;
    }
    sData = tokenIt->second;

    output = "";
    output = sData->Value;
    return true;
}

const std::string ScriptToken::EncodeSpecialCharString(const std::string& input)
{
    std::string transChar;
    size_t strLen = input.length();

    for (size_t i = 0; i < strLen - 1; i++) {
        char c = input[i];

        // Check if we need to record.
        if (EncodeSpecialChar(c, transChar)) {
            // Save our value.
            _ResultBuffer->Append(transChar.c_str());
            continue;
        }

        // Save our value.
        _ResultBuffer->AppendByte(c);
    }

    const std::string result = _ResultBuffer->CStr();
    _ResultBuffer->Clear();
    _TranslateBuffer->Clear();

    return result;
}

bool ScriptToken::DecodeSpecialChar(const std::string& input, char& output)
{
    for (auto tokenIt : _SpecTokens) {
        TokenMeta* token = tokenIt.second;

        if (token->Value == input) {
            output = token->AltName;
            return true;
        }
    }

    output = '\0';
    return false;
}

const std::string ScriptToken::DecodeSpecialCharString(const std::string& input)
{
    bool record = false;
    int recordCount = 1;
    size_t strLen = input.length();

    for (size_t i = 0; i < strLen; i++) {
        char c = input[i];
        char peek = '\0';

        if ((i + 1) < strLen) {
            peek = input[i + 1];
        }

        // Check if we need to record.
        if (!record && c == '#') {
            record = !record;
            if (!record) {
                recordCount = 1;
                _TranslateBuffer->Clear();
            }

            // Save our value.
            _TranslateBuffer->AppendByte(c);
            continue;
        }

        // If record is greater than 4 we are probably
        // not inside of a special char.
        if (record) {
            _TranslateBuffer->AppendByte(c);
            recordCount++;

            // Clear our options.
            if (recordCount == 4 && c !=  '#') {
                record = false;
                recordCount = 1;
                _ResultBuffer->Append(_TranslateBuffer->CStr());
                _TranslateBuffer->Clear();
                continue;
            }

            // Try to decode our string.
            if (c == '#') {
                // Attempt to translate our string.
                char dc;
                std::string tranStr = _TranslateBuffer->CStr();

                if (!DecodeSpecialChar(tranStr, dc)) {
                    _ResultBuffer->Append(tranStr.c_str());
                } else {
                    _ResultBuffer->AppendByte(dc);
                }

                _TranslateBuffer->Clear();
                recordCount = 1;
                record = false;
            }
            continue;
        }

        // Save our value.
        _ResultBuffer->AppendByte(c);
    }

    const std::string result = _ResultBuffer->CStr();
    _ResultBuffer->Clear();
    _TranslateBuffer->Clear();

    return result;
}

void ScriptToken::AddStdToken(TokenType type, const std::string& name, const std::string& value)
{
    // Ensure we don't already have this symbo.
    if (_StdTokens.find(name) != _StdTokens.end()) {
        return;
    }

    auto pToken = new TokenMeta();
    pToken->Name = name;
    pToken->Value = value;
    pToken->Type = type;

    _StdTokens[name] = pToken;
}

void ScriptToken::AddDelToken(TokenType type, const std::string& name, const std::string& value)
{
    // Ensure we don't already have this symbo.
    if (_DelTokens.find(name) != _DelTokens.end()) {
        return;
    }

    TokenMeta* pToken = new TokenMeta();
    pToken->Name = name;
    pToken->Value = value;
    pToken->Type = type;

    _DelTokens[name] = pToken;
}

void ScriptToken::AddSpecialCharToken(TokenType type, const std::string& name, const std::string& value)
{
    // Ensure we don't already have this symbo.
    if (_SpecTokens.find(name) != _SpecTokens.end()) {
        return;
    }

    TokenMeta* pToken = new TokenMeta();
    pToken->Name = "";
    pToken->AltName = (char)name[0];
    pToken->Value = value;
    pToken->Type = type;

    _SpecTokens[name] = pToken;
}

bool ScriptToken::Release()
{
    TokenMeta* data;
    for (auto tokensIt : _StdTokens) {
        data = tokensIt.second;
        delete data;
    }
    _StdTokens.clear();

    for (auto tokensIt : _DelTokens) {
        data = tokensIt.second;
        delete data;
    }
    _DelTokens.clear();

    for (auto tokensIt : _SpecTokens) {
        data = tokensIt.second;
        delete data;
    }
    _SpecTokens.clear();

    if (!IsNullObject(_TranslateBuffer)) free_ref(_TranslateBuffer);
    if (!IsNullObject(_ResultBuffer)) free_ref(_ResultBuffer);

    return true;
}