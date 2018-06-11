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

TokenMeta ScriptToken::Empty;

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

    _TranslateBuffer = mem_alloc_ref(StringBuffer);
    _ResultBuffer = mem_alloc_ref(StringBuffer);
}

const bool ScriptToken::IsObject(const char input)
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

const bool ScriptToken::IsOperator(const char input)
{
    auto token = FindToken(TokenType::Operator, input);
    return !token->IsEmpty();
}

const bool ScriptToken::IsOperator(const char* input)
{
    auto token = FindToken(TokenType::Operator, input);
    return !token->IsEmpty();
}

const bool ScriptToken::IsIdentifier(const char input)
{
    auto token = FindToken(TokenType::Identifier, input);
    return !token->IsEmpty();
}

const bool ScriptToken::IsIdentifier(const char* input)
{
    auto token = FindToken(TokenType::Identifier, input);
    return !token->IsEmpty();
}

const bool ScriptToken::IsDelimiter(const char input)
{
    auto token = FindToken(TokenType::Delimiter, input);
    return !token->IsEmpty();
}

const bool ScriptToken::IsSpecialChar(const char input)
{
    auto token = FindToken(TokenType::SpecialChar, input);
    return !token->IsEmpty();
}

bool ScriptToken::EncodeSpecialChar(const char input, std::string& output)
{
    std::string inputStr = std::string(1, input);

    auto tokenIt = FindToken(TokenType::SpecialChar, inputStr.c_str());
    if (tokenIt->IsEmpty()) {
        return false;
    }

    output = tokenIt->Value;
    return true;
}

const std::string ScriptToken::EncodeSpecialCharString(const char* input)
{
    std::string transChar;
    size_t strLen = strlen(input);

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

bool ScriptToken::DecodeSpecialChar(const char* input, char& output)
{
    for (auto& tokenIt : _SpecTokens) {
        if (tokenIt->Value == input) {
            output = tokenIt->AltName;
            return true;
        }
    }

    output = '\0';
    return false;
}

const std::string ScriptToken::DecodeSpecialCharString(const char* input)
{
    bool record = false;
    int recordCount = 1;
    size_t strLen = strlen(input);

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

                if (!DecodeSpecialChar(tranStr.c_str(), dc)) {
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

void ScriptToken::AddStdToken(TokenType type, const char* name, const char* value)
{
    // Ensure we don't already have this symbol.
    auto tokenIt = FindToken(TokenType::Identifier, name);
    if (!tokenIt->IsEmpty()) {
        return;
    }

    auto token = alloc_bytes(TokenMeta);
    memcpy(token->Name, name, strlen(name));
    memcpy(token->Value, value, strlen(value));
    token->Type = type;

    _StdTokens.push_back(token);
}

void ScriptToken::AddDelToken(TokenType type, const char* name, const char* value)
{
    // Ensure we don't already have this symbol.
    auto tokenIt = FindToken(TokenType::Delimiter, name);
    if (!tokenIt->IsEmpty()) {
        return;
    }

    auto token = alloc_bytes(TokenMeta);
    memcpy(token->Name, name, strlen(name));
    memcpy(token->Value, value, strlen(value));
    token->Type = type;

    _DelTokens.push_back(token);
}

void ScriptToken::AddSpecialCharToken(TokenType type, const char* name, const char* value)
{
    // Ensure we don't already have this symbol.
    auto tokenIt = FindToken(TokenType::SpecialChar, name);
    if (!tokenIt->IsEmpty()) {
        return;
    }

    auto token = alloc_bytes(TokenMeta);
//    token->Name = "";
    token->AltName = name[0];
    memcpy(token->Value, value, strlen(value));
    token->Type = type;

    _SpecTokens.push_back(token);
}

TokenMeta* ScriptToken::FindToken(TokenType type, const char* token)
{
    size_t size;
    switch ((int)type) {
        case (int)TokenType::Delimiter:
            size = _DelTokens.size();
            for (unsigned i = 0; i < size; i++) {
                if (strcmp(_DelTokens[i]->Name, token) == 0) {
                    return _DelTokens[i];
                }
            }
            break;
        case (int)TokenType::SpecialChar:
            size = _SpecTokens.size();
            for (unsigned i = 0; i < size; i++) {
                if (strcmp(_SpecTokens[i]->Name, token) == 0) {
                    return _SpecTokens[i];
                }
            }
            break;
        default:
            size = _StdTokens.size();
            for (unsigned i = 0; i < size; i++) {
                if (strcmp(_StdTokens[i]->Name, token) == 0) {
                    return _StdTokens[i];
                }
            }
    }
    return &ScriptToken::Empty;
}

TokenMeta* ScriptToken::FindToken(TokenType type, const char token)
{
    size_t size;
    switch ((int)type) {
        case (int)TokenType::Delimiter:
            size = _DelTokens.size();
            for (unsigned i = 0; i < size; i++) {
                if (*_DelTokens[i]->Name == token) {
                    return _DelTokens[i];
                }
            }
            break;
        case (int)TokenType::SpecialChar:
            size = _SpecTokens.size();
            for (unsigned i = 0; i < size; i++) {
                if (*_SpecTokens[i]->Name == token) {
                    return _SpecTokens[i];
                }
            }
            break;
        default:
            size = _StdTokens.size();
            for (unsigned i = 0; i < size; i++) {
                if (*_StdTokens[i]->Name == token) {
                    return _StdTokens[i];
                }
            }
    }
    return &ScriptToken::Empty;
}

bool ScriptToken::Release()
{
    for (auto entryIt : _StdTokens) {
        free_bytes(entryIt);
    }
    for (auto entryIt : _DelTokens) {
        free_bytes(entryIt);
    }
    for (auto entryIt : _SpecTokens) {
        free_bytes(entryIt);
    }

    _StdTokens.clear();
    _DelTokens.clear();
    _SpecTokens.clear();

    mem_free_ref(StringBuffer, _TranslateBuffer);
    mem_free_ref(StringBuffer, _ResultBuffer);

    return true;
}
