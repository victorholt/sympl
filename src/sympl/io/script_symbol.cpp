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
#include <sympl/io/script_symbol.h>
#include <sympl/core/string_buffer.h>
sympl_namespaces

ScriptSymbol::ScriptSymbol()
{
    // Setup the symbols.
    AddStdSymbol(SymbolType::Operator, "=", "=");
    AddStdSymbol(SymbolType::Operator, "+", "+");
    AddStdSymbol(SymbolType::Operator, "-", "-");
    AddStdSymbol(SymbolType::Operator, "/", "/");
    AddStdSymbol(SymbolType::Operator, "*", "*");
    AddStdSymbol(SymbolType::Operator, ">", ">");
    AddStdSymbol(SymbolType::Operator, "<", "<");
    AddStdSymbol(SymbolType::Operator, ">=", ">=");
    AddStdSymbol(SymbolType::Operator, "<=", "<=");
    AddStdSymbol(SymbolType::Operator, "|", "|");
    AddStdSymbol(SymbolType::Operator, "&", "&");
    AddStdSymbol(SymbolType::Operator, "!", "!");

    AddStdSymbol(SymbolType::Operator, "||", "||");
    AddStdSymbol(SymbolType::Operator, "&&", "&&");
    AddStdSymbol(SymbolType::Operator, "!=", "!=");
    AddStdSymbol(SymbolType::Operator, "==", "==");

    AddStdSymbol(SymbolType::Identifier, "{", "{");
    AddStdSymbol(SymbolType::Identifier, "}", "}");
    AddStdSymbol(SymbolType::Identifier, "[", "[");
    AddStdSymbol(SymbolType::Identifier, "]", "]");
    AddStdSymbol(SymbolType::Identifier, "@", "@");

    AddDelSymbol(SymbolType::Delimiter, ",", ",");
    AddDelSymbol(SymbolType::Delimiter, ";", ";");

    AddSpecialCharSymbol(SymbolType::SpecialChar, ",", "#c#");
    AddSpecialCharSymbol(SymbolType::SpecialChar, ";", "#sc#");
    AddSpecialCharSymbol(SymbolType::SpecialChar, "\'", "#q#");
    AddSpecialCharSymbol(SymbolType::SpecialChar, "\"", "\"");

    _TranslateBuffer = alloc_ref(StringBuffer);
    _TranslateBuffer->Resize(200);

    _ResultBuffer = alloc_ref(StringBuffer);
    _ResultBuffer->Resize(1000);
}

ScriptSymbol::~ScriptSymbol()
{
    SymbolMeta* data;
    for (auto symbolsIt : _StdSymbols) {
        data = symbolsIt.second;
        delete data;
    }
    _StdSymbols.clear();

    for (auto symbolsIt : _DelSymbols) {
        data = symbolsIt.second;
        delete data;
    }
    _DelSymbols.clear();

    for (auto symbolsIt : _SpecSymbols) {
        data = symbolsIt.second;
        delete data;
    }
    _SpecSymbols.clear();

    if (!IsNullObject(_TranslateBuffer)) free_ref(StringBuffer, _TranslateBuffer);
    if (!IsNullObject(_ResultBuffer)) free_ref(StringBuffer, _ResultBuffer);
}

const bool ScriptSymbol::IsType(SymbolType type, const std::string& input) const
{
    auto symbolValue = _StdSymbols.find(input);
    if (type != SymbolType::Delimiter && symbolValue == _StdSymbols.end()) {
        return false;
    } else if (type == SymbolType::Delimiter) {
        symbolValue = _DelSymbols.find(input);
        if (symbolValue == _DelSymbols.end()) {
            return false;
        }
    }

    return (symbolValue->second->Type == type);
}

const bool ScriptSymbol::IsType(SymbolType type, const char input) const
{
    std::string inputStr = "";
    inputStr += input;

    if (type == SymbolType::SpecialChar) {
        return (_SpecSymbols.find(inputStr) != _SpecSymbols.end());
    }

    return IsType(type, inputStr);
}

const bool ScriptSymbol::IsObject(const char input) const
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

const bool ScriptSymbol::IsOperator(const char input) const
{
    return IsOperator(std::string(1, input));
}

const bool ScriptSymbol::IsOperator(const std::string& input) const
{
    return IsType(SymbolType::Operator, input);
}

const bool ScriptSymbol::IsIdentifier(const char input) const
{
    return IsIdentifier(std::string(1, input));
}

const bool ScriptSymbol::IsIdentifier(const std::string& input) const
{
    return IsType(SymbolType::Identifier, input);
}

const bool ScriptSymbol::IsKeyword(const std::string& input) const
{
    return IsType(SymbolType::Keyword, input);
}

const bool ScriptSymbol::IsDelimiter(const char input) const
{
    return IsType(SymbolType::Delimiter, input);
}

const bool ScriptSymbol::IsSpecialChar(const char input) const
{
    return IsType(SymbolType::SpecialChar, input);
}

bool ScriptSymbol::EncodeSpecialChar(const char input, std::string& output)
{
    std::string inputStr = "";
    inputStr += input;

    SymbolMeta* sData;
    auto symbolIt = _SpecSymbols.find(inputStr);
    if (symbolIt == _SpecSymbols.end()) {
        return false;
    }
    sData = symbolIt->second;

    output = "";
    output = sData->Value;
    return true;
}

const std::string ScriptSymbol::EncodeSpecialCharString(const std::string& input)
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

bool ScriptSymbol::DecodeSpecialChar(const std::string& input, char& output)
{
    for (auto symbolIt : _SpecSymbols) {
        SymbolMeta* symbol = symbolIt.second;

        if (symbol->Value == input) {
            output = symbol->AltName;
            return true;
        }
    }

    output = '\0';
    return false;
}

const std::string ScriptSymbol::DecodeSpecialCharString(const std::string& input)
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

void ScriptSymbol::AddStdSymbol(SymbolType type, const std::string& name, const std::string& value)
{
    // Ensure we don't already have this symbo.
    if (_StdSymbols.find(name) != _StdSymbols.end()) {
        return;
    }

    auto pSymbol = new SymbolMeta();
    pSymbol->Name = name;
    pSymbol->Value = value;
    pSymbol->Type = type;

    _StdSymbols[name] = pSymbol;
}

void ScriptSymbol::AddDelSymbol(SymbolType type, const std::string& name, const std::string& value)
{
    // Ensure we don't already have this symbo.
    if (_DelSymbols.find(name) != _DelSymbols.end()) {
        return;
    }

    SymbolMeta* pSymbol = new SymbolMeta();
    pSymbol->Name = name;
    pSymbol->Value = value;
    pSymbol->Type = type;

    _DelSymbols[name] = pSymbol;
}

void ScriptSymbol::AddSpecialCharSymbol(SymbolType type, const std::string& name, const std::string& value)
{
    // Ensure we don't already have this symbo.
    if (_SpecSymbols.find(name) != _SpecSymbols.end()) {
        return;
    }

    SymbolMeta* pSymbol = new SymbolMeta();
    pSymbol->Name = "";
    pSymbol->AltName = (char)name[0];
    pSymbol->Value = value;
    pSymbol->Type = type;

    _SpecSymbols[name] = pSymbol;
}
