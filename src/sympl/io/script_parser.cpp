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
#include <sympl/script/interpreter.h>

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
}

ScriptParser::~ScriptParser()
{
    free_ref(StringBuffer, _CurrentIdentifierBuffer);
    free_ref(StringBuffer, _CurrentObjectBuffer);
    free_ref(StringBuffer, _CurrentValueBuffer);
}

void ScriptParser::Parse(Interpreter* interpreter)
{
    assert(!IsNullObject(interpreter) && "Invalid interpreter given during parse!");

    _ClearBuffers();

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

    _Reader = reader;
    _RecordingString = false;
    _CharLocation = 0;

    // Ensure we can end on a ; delimeter.
    if (_Reader->GetBuffer()->LastByte() != ';') {
        _Reader->GetBuffer()->AppendByte(';');
    }

    // Go through the buffer and parse out the script.
    while (_CharLocation < _Reader->GetBuffer()->Length() - 1) {
        previousChar = currentChar;
        currentChar = _Reader->GetBuffer()->Get(_CharLocation);
        nextChar = _Reader->GetBuffer()->Get(_CharLocation + 1);
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
        if (!_RecordingString && currentChar == '{') {
            _OpenScope();

            bufferIndex = 0;
            _ClearBuffers();
            continue;
        }

        // Attempt to close the current scope.
        if (!_RecordingString && currentChar == '}') {
            _CloseScope();

            bufferIndex = 0;
            _ClearBuffers();
            continue;
        }

        if (!_RecordingString && currentChar == '(') {
            // Discovering a new method definition.
            if (_ScanMode == ParserScanMode::VarName) {
                bufferIndex = 0;
                _UpdateScanMode();
                continue;
            }
        }

        if (!_RecordingString && currentChar == '#') {
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

            // Continue to the next iteration if we don't have any inputs based on the parser
            // scan mode.
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

            // If the next character IS an operator, we should update
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
    // Determine our variable type based on the identifier value.
    // Classes are essential data objects that hold variables
    // and functions, thus, we'll never have a statement for them
    // and they should reset the scanner and update the scope.
    ScriptObjectType type = ScriptObjectType::Object;
    if (_CurrentIdentifierBuffer->Equals("var")) {
        type = ScriptObjectType::Variable;
    } else if (_CurrentIdentifierBuffer->Equals("func")) {
        type = ScriptObjectType::Method;
    } else if (_CurrentIdentifierBuffer->Equals("class")) {
        type = ScriptObjectType::Object;
    }

    // Attempt to find the object if it already exists and we're just wanting
    // to assign it a new value.
    ScriptObject* existingObject;
    if (_TryFindObject(_CurrentObjectBuffer->CStr(), existingObject)) {
        _CurrentObject = existingObject;
        return;
    }

    // Create our object based on if we have a scope. If no scope
    // is present we will not have a parent.
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

        // Ensure we aren't about to open up a scope.
        if (currentChar == '{') {
            _CurrentObjectBuffer->Clear();
            return;
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
            } else if (_CurrentObjectBuffer->Equals("void")) {
                to_method(_CurrentObject.Ptr())->SetReturnType(MethodReturnType::Void);
            } else {
                assert(false && "Unknown method return type!");
            }
            _CurrentObjectBuffer->Clear();
            return;
        }

        // During 'searchReturnType' we don't want to append the '#' character.
        if (currentChar != '#' && currentChar != ',') {
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
    stat->SetString(_CurrentValueBuffer);

    // Add to the interpreter's command list.

     // Check to see if we're inside of a scope.
    if (_CurrentScopeObject.IsValid()) {
        auto parent = _CurrentScopeObject->GetParent();
        if (parent->GetType() == ScriptObjectType::Method) {
            to_method(parent.Ptr())->AddStatement(_CurrentObject.Ptr(), stat.Ptr());
        }
    } else {
        _Interpreter->AddCommand(_CurrentObject.Ptr(), stat.Ptr());
    }
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
                ScriptObject* existingObject;
                assert(_TryFindObject(_CurrentIdentifierBuffer->CStr(), existingObject) && "Unknown variable assignment attempt!");

                // Update the scan mode build the object (by re-calling UpdateScanMode).
                _ScanMode = ParserScanMode::VarName;
                _CurrentObjectBuffer->Clear();
                _CurrentObjectBuffer->Append(_CurrentIdentifierBuffer->CStr());

                // Set the identifier.
                _CurrentIdentifierBuffer->Clear();
                if (existingObject->GetType() == ScriptObjectType::Method) {
                    _CurrentIdentifierBuffer->Append("func");
                } else {
                    _CurrentIdentifierBuffer->Append("var");
                }

                _BuildObject();
                _ScanMode = ParserScanMode::Value;
                return;
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
    ScriptObject* output;
    return _TryFindObject(objectName, output);
}

bool ScriptParser::_TryFindObject(const char* objectName, ScriptObject*& output)
{
    if (!_CurrentScopeObject.IsValid()) {
        output = SymplVMInstance->FindObject(
            fmt::format(".{0}", objectName)
        );
        return (!IsNullObject(output) && !output->IsEmpty());
    }

    output = SymplVMInstance->FindObject(
        fmt::format("{0}.{1}", _CurrentScopeObject.Ptr()->GetPath(), objectName)
    );

    return (!IsNullObject(output) && !output->IsEmpty());
}

void ScriptParser::_ClearBuffers()
{
    _CurrentObjectBuffer->Clear();
    _CurrentValueBuffer->Clear();
    _CurrentIdentifierBuffer->Clear();
    memset(_CurrentOperator, 0, 3);
}
