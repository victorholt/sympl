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
#include <sympl/io/script_parser.h>
#include <sympl/script/sympl_vm.h>
#include <sympl/core/sympl_number_helper.h>

#include <fmt/format.h>
sympl_namespaces

ScriptParser::ScriptParser()
{
    _CurrentObjectBuffer = alloc_ref(StringBuffer);
    _CurrentObjectBuffer->Resize(256);

    _CurrentValueBuffer = alloc_ref(StringBuffer);
    _CurrentValueBuffer->Resize(2000);

    _StatementBuffer = alloc_ref(StringBuffer);
    _StatementBuffer->Resize(512);

    _ClearBuffers();
}

ScriptParser::~ScriptParser()
{
    free_ref(StringBuffer, _CurrentObjectBuffer);
    free_ref(StringBuffer, _CurrentValueBuffer);
    free_ref(StringBuffer, _StatementBuffer);
}

void ScriptParser::ParseFile(const char* filePath)
{
    auto reader = alloc_ref(ScriptReader);
    reader->ReadFile(filePath);
    _ParseBuffer(reader);
    free_ref(ScriptReader, reader);
}

void ScriptParser::ParseString(const char* str)
{
    auto reader = alloc_ref(ScriptReader);
    reader->ReadString(str);
    _ParseBuffer(reader);
    free_ref(ScriptReader, reader);
}

void ScriptParser::_ParseBuffer(ScriptReader* reader)
{
    size_t charLocation = 0;
    int nestLevel = 0;
    int currentLineNumber = 0;
    unsigned bufferIndex = 0;
    char currentChar = '\0';
    char previousChar = '\0';
    char nextChar = '\0';
    bool recording = false;
    bool valueMode = false;
    bool inArray = false;
    bool grabbingLineNumber = false;
    std::locale loc;

    while (charLocation < reader->GetBuffer()->Length()) {
        previousChar = currentChar;
        currentChar = reader->GetBuffer()->Get(charLocation);
        nextChar = reader->GetBuffer()->Get(charLocation + 1);
        charLocation++;

        // Check if we're at the end of a statement and clear the buffers.
        if (currentChar == ';') {
            _CurrentValueBuffer->AppendByte(';');
            _UpdateObjectValue();

            _ClearBuffers();
            bufferIndex = 0;
            _ScanMode = ParserScanMode::Type;

            continue;
        }

        // Grab the current line number.
        if (currentChar == '[' && nextChar == '|') {
            grabbingLineNumber = true;
            continue;
        }
        if (grabbingLineNumber) {
            if (previousChar == '|') {
                currentLineNumber = static_cast<int>(currentChar);
            }
            if (currentChar == ']') {
                grabbingLineNumber = false;
            }
            continue;
        }
        // End grabbing the line number.

        // Check if we need to skip.
        if (currentChar == '#' && !recording && _ScanMode != ParserScanMode::Value) {
            bufferIndex = 0;
            _UpdateScanMode();
            continue;
        }

        if (_ScanMode == ParserScanMode::Type) {
            assert(bufferIndex < 6 && "Unknown identifier!");
            _CurrentIdentifier[bufferIndex] = currentChar;
        } else if (_ScanMode == ParserScanMode::VarName) {
            assert(bufferIndex < 256 && "Object name too long!");
            _CurrentObjectBuffer->AppendByte(currentChar);
        } else if (_ScanMode == ParserScanMode::Value) {
            _CurrentValueBuffer->AppendByte(currentChar);
        }
        bufferIndex++;
    }
}

void ScriptParser::_BuildObject()
{
    ScriptObjectType type = ScriptObjectType::Object;
    if (strcmp("var", _CurrentIdentifier) == 0) {
        type = ScriptObjectType::Variable;
    } else if (strcmp("func", _CurrentIdentifier) == 0) {
        type = ScriptObjectType::Method;
    }

    _CurrentObject = SymplVMInstance->CreateObject(_CurrentObjectBuffer->CStr(), type);

}

void ScriptParser::_BuildStatement(ScriptStatement* stat)
{
    StatementOperator currentOp = StatementOperator::Equals;
    bool isString = false;
    bool recording = false;
    int index = 0;

    while (index < _CurrentValueBuffer->Length()) {
        char currentChar = _CurrentValueBuffer->Get(index);
        index++;

        if (!recording && (currentChar == '#' || currentChar == ';')) {
            std::string statementStr = _StatementBuffer->CStr();

            if (_StatementBuffer->Length() > 0) {
                // Check if we're starting a string.
                if (currentChar == '"') {
                    recording = true;
                    isString = true;
                    continue;
                }

                // Check if we have a current operator.
                if (_Symbol.IsOperator(statementStr)) {
                    currentOp = _SymbolToOp(statementStr);
                    _StatementBuffer->Clear();
                    continue;
                }

                // Check if we have an object.
                if (!isString) {
                    /// This is an existing object that we should add to the statement.
                    auto obj = SymplVMInstance->FindObject(fmt::format(".{0}", _StatementBuffer->CStr()).c_str());
                    if (!IsNullObject(obj) && !obj->IsEmpty()) {
                        stat->Add(obj, currentOp);
                    } else {
                        const char* strVal = _StatementBuffer->CStr();

                        // Handle boolean value.
                        if (strcmp(strVal, "true") == 0 || strcmp(strVal, "false") == 0) {
                            stat->SetType(StatementType::Bool);
                            stat->Add(strcmp(strVal, "true") == 0 ? true : false, currentOp);
                        } else {
                            // Handle constant value.
                            int intVal;
                            float floatVal;

                            if (NumberHelper::TryParseInt(strVal, &intVal)) {
                                stat->SetType(StatementType::Integer);
                                stat->Add(intVal, currentOp);
                            } else if (NumberHelper::TryParseFloat(strVal, &floatVal)) {
                                stat->SetType(StatementType::Float);
                                stat->Add(floatVal, currentOp);
                            } else {
                                stat->SetType(StatementType::String);
                                stat->Add(_StatementBuffer->CStr(), currentOp);
                            }
                        }
                    }
                } else {
                    /// Handle case where this is a string.
                    stat->SetType(StatementType::String);
                    stat->Add(_StatementBuffer->CStr(), currentOp);
                }
            }

            isString = false;
            _StatementBuffer->Clear();
        } else {
            // Check if we need to start/stop recording a string.
            if (currentChar == '"') {
                recording = !recording;
                continue;
            }

            _StatementBuffer->AppendByte(currentChar);
        }
    }
}

void ScriptParser::_UpdateObjectValue()
{
    // Determine how many variables are part of the statement.
    SharedRef<ScriptStatement> stat = alloc_ref(ScriptStatement);
    _CurrentObject->SetValue(stat.Ptr());

    _BuildStatement(stat.Ptr());
}

void ScriptParser::_UpdateScanMode()
{
    switch ((int)_ScanMode) {
        case (int)ParserScanMode::Type:
            if (strlen(_CurrentIdentifier) == 0) {
                return;
            }
            _ScanMode = ParserScanMode::VarName;
            break;
        case (int)ParserScanMode::VarName:
            if (_CurrentObjectBuffer->Length() == 0) {
                return;
            }
            _ScanMode = ParserScanMode::Value;
            _BuildObject();
            break;
        // case ParserScanMode::Value:
        //     _BuildObject();
        //     break;
    }
}

StatementOperator ScriptParser::_SymbolToOp(const std::string& symbol)
{
    // Ensure the symbol is an operator we can parse.
    if (!_Symbol.IsOperator(symbol)) {
        return StatementOperator::None;
    }

    if (symbol == "=") {
        return StatementOperator::Equals;
    }
    if (symbol == "+") {
        return StatementOperator::Add;
    }
    if (symbol == "-") {
        return StatementOperator::Subtract;
    }
    if (symbol == "/") {
        return StatementOperator::Divide;
    }
    if (symbol == "*") {
        return StatementOperator::Multiply;
    }
    if (symbol == ">") {
        return StatementOperator::GreaterThan;
    }
    if (symbol == "<") {
        return StatementOperator::LessThan;
    }
    if (symbol == ">=") {
        return StatementOperator::GreaterEqualThan;
    }
    if (symbol == "<=") {
        return StatementOperator::LessEqualThan;
    }

    return StatementOperator::None;
}

void ScriptParser::_ClearBuffers()
{
    _CurrentObjectBuffer->Clear();
    _CurrentValueBuffer->Clear();
    _StatementBuffer->Clear();
    memset(_CurrentIdentifier, 0, 6);
    memset(_CurrentOperator, 0, 3);
}
