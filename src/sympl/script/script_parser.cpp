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
#include <sympl/script/script_parser.h>
#include <sympl/script/script_vm.h>
#include <sympl/script/script_object.h>
#include <sympl/script/script_statement.h>
#include <sympl/script/script_method.h>
#include <sympl/script/interpreter.h>
#include <sympl/script/method_registry.h>
#include <sympl/util/number_helper.h>

#include <fmt/format.h>
sympl_namespaces

ScriptParser::ScriptParser()
{
    __Construct();
}

void ScriptParser::__Construct()
{
    _Reader = nullptr;

    _CurrentIdentifierBuffer = alloc_ref(StringBuffer);
    _CurrentObjectBuffer = alloc_ref(StringBuffer);

    _CurrentValueBuffer = alloc_ref(StringBuffer);
    _CurrentValueBuffer->Resize(512);

    _Symbol = ScriptVMInstance->GetScriptToken();
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
    //if (_Reader->GetBuffer()->LastByte() != ';') {
    //    _Reader->GetBuffer()->AppendByte(';');
    //}

    // Add an EOF var at the end. Dirty hack to figure out
    // that we're really at the end.
    _Reader->GetBuffer()->Append("#var#___EOF___#;");

    // Go through the buffer and parse out the script.
    while (_CharLocation < _Reader->GetBuffer()->Length()) {
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
            // Set the argument string if this is a method.
            if (_CurrentObject.IsValid() && _CurrentObject->GetType() == ScriptObjectType::Method) {
                to_method(_CurrentObject.Ptr())->SetArgString(_CurrentValueBuffer);
            }

            _OpenScope();

            bufferIndex = 0;
            _ClearBuffers();
            continue;
        }

        // Attempt to close the current scope.
        if (!_RecordingString && currentChar == '}') {
            _CloseScope();

            // Check if this is an immediate method.
            if (!_CurrentScopeObject.IsValid()) {
                if (_CurrentObject.IsValid() && _CurrentObject->GetType() == ScriptObjectType::Method && to_method(_CurrentObject.Ptr())->IsImmediate()) {
                    SharedPtr<ScriptStatement> stat = alloc_ref(ScriptStatement);
                    stat->SetString(to_method(_CurrentObject.Ptr())->GetArgString());
                    _Interpreter->AddCommand(_CurrentObject.Ptr(), stat.Ptr());
                }
            } else {
                if (_CurrentObject.IsValid() && _CurrentObject->GetType() == ScriptObjectType::Method && to_method(_CurrentObject.Ptr())->IsImmediate()) {
                    auto parent = _CurrentScopeObject->GetParent();
                    if (parent->GetType() == ScriptObjectType::Method) {
                        SharedPtr<ScriptStatement> stat = alloc_ref(ScriptStatement);
                        stat->SetString(to_method(_CurrentObject.Ptr())->GetArgString());
                        to_method(parent.Ptr())->AddStatement(_CurrentObject.Ptr(), stat.Ptr());
                    }
                }
            }

            bufferIndex = 0;
            _ClearBuffers();
            continue;
        }

        if (!_RecordingString && currentChar == '(') {
            // Discovering a new method definition.
            if (_ScanMode == ParserScanMode::Type || _ScanMode == ParserScanMode::VarName) {
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
            if (_Symbol->IsOperator(nextChar)) {
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
    ScriptObject* existingObject = _FindObject(_CurrentObjectBuffer->CStr());
    if (!existingObject->IsEmpty()) {
        _CurrentObject = existingObject;

        // If this object exists as an immediate method we should clone the original.
        if (_CurrentObject->GetType() == ScriptObjectType::Method && to_method(_CurrentObject.Ptr())->IsImmediate()) {
            _CurrentObject = _CurrentObject->Clone(_CurrentScopeObject.Ptr(), true);
            _CurrentObjectBuffer->Clear();
            _CurrentObjectBuffer->Append(_CurrentObject->GetName());
        }

        return;
    }

    // Create our object based on if we have a scope. If no scope
    // is present we will not have a parent.
    if (!_CurrentScopeObject.IsValid()) {
        _CurrentObject = ScriptVMInstance->CreateObject(_CurrentObjectBuffer->CStr(), type);
        if (type == ScriptObjectType::Variable) {
            ScriptVMInstance->GetContext()->AddVar(_CurrentObject.Ptr());
        }
    } else {
        _CurrentObject = ScriptVMInstance->CreateObject(_CurrentObjectBuffer->CStr(), type, _CurrentScopeObject.Ptr());
        if (type == ScriptObjectType::Variable) {
            _CurrentScopeObject->GetContext()->AddVar(_CurrentObject.Ptr());
        }
    }

    if (type == ScriptObjectType::Method) {
        ScriptVMInstance->GetMethodRegistry()->AddMethod(_CurrentObject.Ptr());
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
                auto argObj = ScriptVMInstance->CreateObject(_CurrentObjectBuffer->CStr(), ScriptObjectType::Variable, _CurrentScopeObject.Ptr());
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

        if (currentChar != '#' && currentChar != ',') {
            _CurrentObjectBuffer->AppendByte(currentChar);
        }
    }

    std::cout << _CurrentObjectBuffer->CStr() << std::endl;

    // The while loop should quit before we reach this call.
    assert(_CharLocation < _Reader->GetBuffer()->Length() && "Invalid method parsing error!");
}

void ScriptParser::_UpdateObjectValue()
{
    // Create our statement.
    SharedPtr<ScriptStatement> stat = alloc_ref(ScriptStatement);

    // Add to the interpreter's command list.

    // Check if this is an immediate method.
    if (!_CurrentScopeObject.IsValid()) {
        if (_CurrentObject.IsValid() && _CurrentObject->GetType() == ScriptObjectType::Method && to_method(_CurrentObject.Ptr())->IsImmediate()) {
            // We're setting the value buffer as this was a build-in method.
            _CurrentValueBuffer->PrependByte('(');
            stat->SetString(_CurrentValueBuffer);
            _Interpreter->AddCommand(_CurrentObject.Ptr(), stat.Ptr());
            return;
        }
    } else if (_CurrentScopeObject.IsValid()) {
        if (_CurrentObject.IsValid() && _CurrentObject->GetType() == ScriptObjectType::Method && to_method(_CurrentObject.Ptr())->IsImmediate()) {
            _CurrentValueBuffer->PrependByte('(');
        }
    }

    // Check to see if we're inside of a scope.
    stat->SetString(_CurrentValueBuffer);
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
                ScriptObject* existingObject = _FindObject(_CurrentIdentifierBuffer->CStr());
                if (existingObject->IsEmpty()) {
                    assert(!existingObject->IsEmpty() && "Unknown variable assignment attempt!");
                }

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
    _CurrentScopeObject = _CurrentObject->FindChildByName(SYMPL_SCOPE_NAME);
    if (_CurrentScopeObject->IsEmpty()) {
        _CurrentScopeObject = ScriptVMInstance->CreateObject(SYMPL_SCOPE_NAME, ScriptObjectType::Object, _CurrentObject.Ptr());
    }
    _ScanMode = ParserScanMode::Type;
}

void ScriptParser::_CloseScope()
{
    if (_CurrentScopeObject.IsValid()) {
        _CurrentObject = _CurrentScopeObject->GetParent().Ptr();
        _CurrentScopeObject.Release();
    } else {
        _CurrentObject = _CurrentObject->GetParent().Ptr();
    }

    // Set the new scope.
    if (_CurrentObject->GetParent().IsValid()) {
        _CurrentScopeObject = _CurrentObject->GetParent().Ptr();
    }

    _ScanMode = ParserScanMode::Type;
}

bool ScriptParser::_ObjectExists(const char* objectName)
{
    ScriptObject* output = _FindObject(objectName);
    return !output->IsEmpty();
}

ScriptObject* ScriptParser::_FindObject(const char* objectName)
{
    ScriptObject* output = &ScriptObject::Empty;
    if (!_CurrentScopeObject.IsValid()) {
        output = ScriptVMInstance->FindObjectByPath(
                fmt::format("{0}", objectName)
        );
        return output;
    }

    return _CurrentScopeObject->TraverseUpFindChildByName(objectName, false);
}

void ScriptParser::_ClearBuffers()
{
    _CurrentObjectBuffer->Clear();
    _CurrentValueBuffer->Clear();
    _CurrentIdentifierBuffer->Clear();
    memset(_CurrentOperator, 0, 3);
}

bool ScriptParser::Release()
{
    free_ref(_CurrentIdentifierBuffer);
    free_ref(_CurrentObjectBuffer);
    free_ref(_CurrentValueBuffer);

    return true;
}
