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
#include <sympl/script/script_method.h>
#include <sympl/core/sympl_number_helper.h>

#include <fmt/format.h>
sympl_namespaces

ScriptParser::ScriptParser()
{
    _Reader = nullptr;

    _CurrentIdentifierBuffer = alloc_ref(StringBuffer);
    _CurrentIdentifierBuffer->Resize(256);

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
    free_ref(StringBuffer, _CurrentIdentifierBuffer);
    free_ref(StringBuffer, _CurrentObjectBuffer);
    free_ref(StringBuffer, _CurrentValueBuffer);
    free_ref(StringBuffer, _StatementBuffer);
}

void ScriptParser::Parse(Interpreter* interpreter)
{
    _Interpreter = interpreter;
    _ParseBuffer(_Interpreter->GetReader());
}

void ScriptParser::_ParseBuffer(ScriptReader* reader)
{
    int _CurrentLine = 0;
    unsigned bufferIndex = 0;
    char currentChar = '\0';
    char previousChar = '\0';
    char nextChar = '\0';
    bool valueMode = false;
    bool inArray = false;
    bool grabbingLineNumber = false;

    _RecordingString = false;
    _CharLocation = 0;

    while (_CharLocation < reader->GetBuffer()->Length()) {
        previousChar = currentChar;
        currentChar = reader->GetBuffer()->Get(_CharLocation);
        nextChar = reader->GetBuffer()->Get(_CharLocation + 1);
        _CharLocation++;

        // Check if we're at the end of a statement and clear the buffers.
        if (currentChar == ';') {
            // If we're attempting to find method arguments and we're
            // ending too soon, error out!
            assert(currentChar == ';' && (_ScanMode == ParserScanMode::VarName || _ScanMode == ParserScanMode::Value) && "Unable to close statement without object name.");

            _CurrentValueBuffer->PrependByte('#');
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
                _CurrentLine = static_cast<int>(currentChar);
            }
            if (currentChar == ']') {
                grabbingLineNumber = false;
            }
            continue;
        }
        // End grabbing the line number.

        // Attempt to open a new scope.
        if (currentChar == '{' && !_RecordingString) {
            _OpenScope();

            bufferIndex = 0;
            _ClearBuffers();
            continue;
        }

        // Attempt to close the current scope.
        if (currentChar == '}' && !_RecordingString) {
            _CloseScope();

            bufferIndex = 0;
            _ClearBuffers();
            continue;
        }

        if (currentChar == '(' && !_RecordingString) {
            // Discovering a new method definition.
            if (_ScanMode == ParserScanMode::VarName) {
                bufferIndex = 0;
                _UpdateScanMode();
                continue;
            } else if (_ScanMode == ParserScanMode::Type || _ScanMode == ParserScanMode::Value) {
                //_CallMethod();
                continue;
            }
        }

        if (currentChar == '#' && !_RecordingString) {
            // If we're a 'return' statement, treat it as a variable
            // so we can add the rest as a statement.
            if (_CurrentIdentifierBuffer->Equals("return")) {
                _CurrentIdentifierBuffer->Clear();
                _CurrentIdentifierBuffer->Append("var");

                // Switch over to varname
                _ScanMode = ParserScanMode::VarName;
                _CurrentObjectBuffer->Append("return");

                bufferIndex = 0;
                _UpdateScanMode();
                continue;
            }

            if (_ScanMode == ParserScanMode::Type && _CurrentIdentifierBuffer->Length() == 0) {
                continue;
            }
            if (_ScanMode == ParserScanMode::VarName && _CurrentObjectBuffer->Length() == 0) {
                continue;
            }
            if (_ScanMode == ParserScanMode::Value && _CurrentValueBuffer->Length() == 0) {
                continue;
            }

            // Update the scan mode.
            if (_ScanMode != ParserScanMode::Value &&
                _ScanMode != ParserScanMode::MethodArgs
            ) {
                bufferIndex = 0;
                _UpdateScanMode();
                continue;
            }
        }

        if (_ScanMode == ParserScanMode::Type) {
            assert(bufferIndex < 256 && "Identifier name too long!");
            _CurrentIdentifierBuffer->AppendByte(currentChar);
        } else if (_ScanMode == ParserScanMode::VarName) {
            assert(bufferIndex < 256 && "Object name too long!");
            _CurrentObjectBuffer->AppendByte(currentChar);

            // If the next character IS an identifier, we should update
            // the scan mode. (x=1 instead of x = 1).
            if (_Symbol.IsOperator(nextChar)) {
                bufferIndex = 0;
                _UpdateScanMode();
                continue;
            }

        } else if (_ScanMode == ParserScanMode::Value) {
            _CurrentValueBuffer->AppendByte(currentChar);
        }
        bufferIndex++;
    }
}

void ScriptParser::_BuildObject()
{
    ScriptObjectType type = ScriptObjectType::Object;
    if (_CurrentIdentifierBuffer->Equals("var")) {
        type = ScriptObjectType::Variable;
    } else if (_CurrentIdentifierBuffer->Equals("func")) {
        type = ScriptObjectType::Method;
    }

    // Ensure we aren't duplicating a variable.
    assert(!_ObjectExists(_CurrentObjectBuffer->CStr()) && "Duplicate variable detected!");

    // Create our object based on if we have a scope.
    if (!_CurrentScopeObject.IsValid()) {
        _CurrentObject = SymplVMInstance->CreateObject(_CurrentObjectBuffer->CStr(), type);
    } else {
        _CurrentObject = SymplVMInstance->CreateObject(_CurrentObjectBuffer->CStr(), type, _CurrentScopeObject.Ptr());
    }
}

void ScriptParser::_BuildMethodArgs()
{
    // Lambdas not supported yet...
    _CurrentIdentifierBuffer->Clear();
    _CurrentIdentifierBuffer->Append("var");

    char currentChar = '\0';
    char previousChar = '\0';
    char nextChar = '\0';
    bool searchReturnType = false;

    _CurrentObjectBuffer->Clear();

    // Open the scope for the arguments.
    _OpenScope();

    while (_CharLocation < _Reader->GetBuffer()->Length()) {
        previousChar = currentChar;
        currentChar = _Reader->GetBuffer()->Get(_CharLocation);
        nextChar = _Reader->GetBuffer()->Get(_CharLocation + 1);
        _CharLocation++;

        // Skip spaces.
        if (currentChar == '#' && !searchReturnType) {
            continue;
        }

        // Attempt to save an argument.
        if (!searchReturnType && !_RecordingString && (currentChar == ',' || currentChar == ')')) {
            // Check to see if we need to create an argument object.
            if (_CurrentObjectBuffer->Length() > 0) {
                auto argObj = SymplVMInstance->CreateObject(_CurrentObjectBuffer->CStr(), ScriptObjectType::Variable, _CurrentScopeObject.Ptr());
                to_method(_CurrentObject.Ptr())->AddArg(argObj);
                _CurrentObjectBuffer->Clear();
            }

            if (currentChar == ')') {
                _CloseScope();
                continue;
            }

            // Ensure we aren't about to open up a scope.
            if (currentChar == '{') {
                return;
            }
        }

        // Search ahead to see if we're a return type.
        if (currentChar == ':' && !searchReturnType && !_RecordingString) {
            searchReturnType = true;
            continue;
        }

        if (currentChar == '#' && searchReturnType && _CurrentObjectBuffer->Length() > 0) {
            if (_CurrentObjectBuffer->Equals("object")) {
                to_method(_CurrentObject.Ptr())->SetReturnType(MethodReturnType::Object);
            } else if (_CurrentObjectBuffer->Equals("string")) {
                to_method(_CurrentObject.Ptr())->SetReturnType(MethodReturnType::String);
            } else if (_CurrentObjectBuffer->Equals("int")) {
                to_method(_CurrentObject.Ptr())->SetReturnType(MethodReturnType::Int);
            } else if (_CurrentObjectBuffer->Equals("float")) {
                to_method(_CurrentObject.Ptr())->SetReturnType(MethodReturnType::Float);
            } else if (_CurrentObjectBuffer->Equals("bool")) {
                to_method(_CurrentObject.Ptr())->SetReturnType(MethodReturnType::Bool);
            } else {
                assert(false && "Unknown method return type!");
            }
            _CurrentObjectBuffer->Clear();
            return;
        }

        // During 'searchReturnType' we don't want to append the '#' character.
        if (currentChar != '#') {
            _CurrentObjectBuffer->AppendByte(currentChar);
        }
    }

    // The while loop should quit before we reach this call.
    assert(_CharLocation < _Reader->GetBuffer()->Length() && "Invalid method parsing error!");
}

void ScriptParser::_UpdateObjectValue()
{
    // Determine how many variables are part of the statement.
    SharedRef<ScriptStatement> stat = alloc_ref(ScriptStatement);
    // Add to the interpreter's command list.

    // _BuildStatement(stat.Ptr());

    // _CurrentObject->SetValue(stat.Ptr());

    // // Check to see if we're inside of a scope.
    // if (_CurrentScopeObject.IsValid()) {
    //     auto parent = _CurrentScopeObject->GetParent();
    //     if (parent->GetType() == ScriptObjectType::Method) {
    //         to_method(parent.Ptr())->AddStatement(_CurrentObject.Ptr(), stat.Ptr());
    //     }
    // } else {
    //     stat.Release();
    // }
}

void ScriptParser::_UpdateScanMode()
{
    switch ((int)_ScanMode) {
        case (int)ParserScanMode::Type:
            if (_CurrentIdentifierBuffer->Length() == 0) {
                return;
            }

            // Validate if this is a proper identifier.
            if (!_CurrentIdentifierBuffer->Equals("var") &&
                !_CurrentIdentifierBuffer->Equals("func")) {
                // Check if this object exists and we're attempting to
                // assign it a value.
                assert(_ObjectExists(_CurrentIdentifierBuffer->CStr()) && "Unknown variable assignment attempt!");

                _ScanMode = ParserScanMode::VarName;
                _CurrentObjectBuffer->Clear();
                _CurrentObjectBuffer->Append(_CurrentIdentifierBuffer->CStr());
                _UpdateScanMode();
            }

            _ScanMode = ParserScanMode::VarName;
            break;
        case (int)ParserScanMode::VarName:
            if (_CurrentObjectBuffer->Length() == 0) {
                return;
            }
            _BuildObject();

            if (_CurrentObject->GetType() == ScriptObjectType::Method) {
                _BuildMethodArgs();
                _ScanMode = ParserScanMode::Type;
            } else {
                _ScanMode = ParserScanMode::Value;
            }
            break;
    }
}

void ScriptParser::_OpenScope()
{
    // Check if we need to exit out of the argument building.
    auto scopeObject = _CurrentObject;
    _CurrentScopeObject = SymplVMInstance->CreateObject(".", ScriptObjectType::Object, scopeObject.Ptr());
    _ScanMode = ParserScanMode::Type;
}

void ScriptParser::_CloseScope()
{
    if (_CurrentScopeObject.IsValid()) {
        _CurrentObject = _CurrentScopeObject->GetParent();
        _CurrentScopeObject.Release();
    } else {
        _CurrentObject = _CurrentObject->GetParent();
    }
    _ScanMode = ParserScanMode::Type;
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

bool ScriptParser::_ObjectExists(const char* objectName)
{
    if (!_CurrentScopeObject.IsValid()) {
        auto foundObj = SymplVMInstance->FindObject(
            fmt::format(".{0}", objectName)
        );
        return (!IsNullObject(foundObj) && !foundObj->IsEmpty());
    }

    auto foundObj = SymplVMInstance->FindObject(
        fmt::format("{0}.{1}", _CurrentScopeObject.Ptr()->GetPath(), objectName)
    );
    return (!IsNullObject(foundObj) && !foundObj->IsEmpty());
}

void ScriptParser::_ClearBuffers()
{
    _CurrentObjectBuffer->Clear();
    _CurrentValueBuffer->Clear();
    _StatementBuffer->Clear();
    _CurrentIdentifierBuffer->Clear();
    memset(_CurrentOperator, 0, 3);
}
