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
#include <sympl/script/interpreter.h>
#include <sympl/script/script_array.h>
#include <sympl/script/script_method.h>

#include <sympl/util/number_helper.h>
#include <sympl/util/string_helper.h>

#include <fmt/format.h>
sympl_namespaces

ScriptParser::ScriptParser()
{
    __Construct();
}

void ScriptParser::__Construct()
{
    _Reader = nullptr;

    _CurrentIdentifierBuffer = mem_alloc_ref(StringBuffer);
    _CurrentObjectBuffer = mem_alloc_ref(StringBuffer);

    _CurrentValueBuffer = mem_alloc_ref(StringBuffer);
    _CurrentValueBuffer->Resize(512);

    _Symbol = SymplRegistry.Get<ScriptToken>();
}

void ScriptParser::Parse(Interpreter* interpreter)
{
    sympl_assert(!IsNullObject(interpreter), "Invalid interpreter given during parse!");

    _ClearBuffers();

    _Interpreter = interpreter;
    _ParseBuffer(_Interpreter->GetReader());
}

void ScriptParser::_ParseBuffer(ScriptReader* reader)
{
    _Reader = reader;
    _RecordingString = false;
    _CharLocation = 0;

    // Go through the buffer and parse out the script.
    while (_CharLocation < _Reader->GetBuffer()->Length()) {
        switch ((int)_ScanMode) {
            case (int)ParserScanMode::Type: {
                _CurrentIdentifierBuffer->Clear();
                _CurrentObjectBuffer->Clear();
                _CurrentValueBuffer->Clear();

                // Parse our line number.
//                _ParseLineNumber();

                // Parse our type.
                _ParseType();
            }
            break;
            case (int)ParserScanMode::VarName: {
                _ParseName();
            }
            break;
            case (int)ParserScanMode::Value: {
                _ParseValue();
                _BuildObject();
                _ClearBuffers();
            }
            break;
        }
    }
}

void ScriptParser::_ParseLineNumber()
{
    char previousChar = '\0';
    char currentChar = '\0';
    char nextChar = '\0';
    bool grabbingLineNumber = false;

    while (_CharLocation < _Reader->GetBuffer()->Length()) {
        previousChar = currentChar;
        currentChar = _Reader->GetBuffer()->Get(_CharLocation);
        nextChar = _Reader->GetBuffer()->Get(_CharLocation + 1);
        _CharLocation++;

        // Grab the current line number.
        if (currentChar == '[' && nextChar == '|') {
            grabbingLineNumber = true;
            continue;
        }
        if (grabbingLineNumber) {
            if (previousChar == '|') {
                _CurrentLine = static_cast<size_t>(currentChar);
            }
            if (currentChar == ']') {
                grabbingLineNumber = false;
            }
        } else {
            _CharLocation--; // Rewind as we didn't find a line number.
            return;
        }
        // End grabbing the line number.
    }

    sympl_assert(false, "Failed to parse line number!");
}

void ScriptParser::_ParseType()
{
    char currentChar = '\0';
    char nextChar = '\0';
    _ScanMode = ParserScanMode::VarName;

    while (_CharLocation < _Reader->GetBuffer()->Length()) {
        currentChar = _Reader->GetBuffer()->Get(_CharLocation);
        nextChar = _Reader->GetBuffer()->Get(_CharLocation + 1);
        _CharLocation++;

        // Open/Close scopes.
        if (currentChar == '{') {
            _OpenScope();
            _ClearBuffers();

            // Ensure we get to the next step after we've discovered the type
            // since this gets reset in _ParseMethodArgs.
            _ScanMode = ParserScanMode::VarName;
            continue;
        }
        if (currentChar == '}') {
            _CloseScope();
            _ClearBuffers();

            // Ensure we get to the next step after we've discovered the type
            // since this gets reset in _ParseMethodArgs.
            _ScanMode = ParserScanMode::VarName;
            continue;
        }

        if (currentChar == '#' && _CurrentIdentifierBuffer->Length() > 0) {
            // Determine if we're defining a type, or using an existing object.
            if (!_CurrentIdentifierBuffer->Equals("var") &&
                !_CurrentIdentifierBuffer->Equals("func") &&
                !_CurrentIdentifierBuffer->Equals("class") &&
                !_CurrentIdentifierBuffer->Equals("return") &&
                !_CurrentIdentifierBuffer->Equals("break") &&
                !_CurrentIdentifierBuffer->Equals("continue")) {
                auto obj = _FindObject(_CurrentIdentifierBuffer->CStr());
                sympl_assert(!obj->IsEmpty(), "Illegal declaration of an object!");

                if (nextChar == ';') {
                    _CharLocation++;
                    _ScanMode = ParserScanMode::Type;
                } else {
                    _CurrentObjectBuffer->Clear();
                    _CurrentObjectBuffer->Append(_CurrentIdentifierBuffer);
                    _ScanMode = ParserScanMode::Value;
                }
            }

            // Handle case where our .
            if (_CurrentIdentifierBuffer->Equals("return")) {
                _CurrentObjectBuffer->Clear();
                _CurrentObjectBuffer->Append(_CurrentIdentifierBuffer);

                _CurrentIdentifierBuffer->Clear();
                _CurrentIdentifierBuffer->Append("var");

                _ScanMode = ParserScanMode::Value;
            }

            return;
        }

        if (currentChar != '#') {
            _CurrentIdentifierBuffer->AppendByte(currentChar);
        }

        // Methods or arrays will need to process as part of the 'value'.
        // We need to ensure that the buffer already has the current char
        // added since we're testing the 'nextChar' value.
        if (nextChar == '(' || nextChar == '[') {
            _CurrentObjectBuffer->Clear();
            _CurrentObjectBuffer->Append(_CurrentIdentifierBuffer);
            _ScanMode = ParserScanMode::Value;
            return;
        }
    }

    if (nextChar == '\0') { // EOF
        return;
    }

    sympl_assert(false, "Missing type for declared variable!");
}

void ScriptParser::_ParseName()
{
    char currentChar = '\0';
    char nextChar = '\0';
    _ScanMode = ParserScanMode::Value;

    while (_CharLocation < _Reader->GetBuffer()->Length()) {
        currentChar = _Reader->GetBuffer()->Get(_CharLocation);
        nextChar = _Reader->GetBuffer()->Get(_CharLocation + 1);
        _CharLocation++;

        if (currentChar == '#') {
            return;
        }

        // Methods or arrays will need to process as part of the 'value'.
        if (nextChar == '(' || nextChar == '[') {
            // Arrays should be marked as arrays in the type.
            if (currentChar == '[') {
                _CurrentIdentifierBuffer->Clear();
                _CurrentIdentifierBuffer->Append("array");
            }
            if (nextChar == '(') {
                _CurrentObjectBuffer->AppendByte(currentChar);
            }
            _ScanMode = ParserScanMode::Value;
            return;
        }

        _CurrentObjectBuffer->AppendByte(currentChar);
    }

    sympl_assert(false, "Incomplete object detected during parsing!");
}

void ScriptParser::_ParseValue()
{
    char currentChar = '\0';
    _ScanMode = ParserScanMode::Type;

    // Whether or not this is an assignment value.
    bool isAssignment = false;

    // Whether or not we're recording a string.
    bool recording = false;

    while (_CharLocation < _Reader->GetBuffer()->Length()) {
        currentChar = _Reader->GetBuffer()->Get(_CharLocation);
        _CharLocation++;

        if (currentChar == ';') {
            return;
        }

        if (currentChar == '=') {
            isAssignment = true;
        }

        if (currentChar == '%' && _CurrentValueBuffer->PeekSearch(SYMPL_STRING_TOKEN, _CharLocation - 1)) {
            recording = !recording;
            _CurrentValueBuffer->Append(SYMPL_STRING_TOKEN);
            _CharLocation = _CharLocation + strlen(SYMPL_STRING_TOKEN) - 1;
            continue;
        }

        // Arrays should be marked as arrays in the type.
        if (!recording && currentChar == '[' && !_CurrentValueBuffer->Contains(SYMPL_STRING_TOKEN)) {
            _CurrentIdentifierBuffer->Clear();
            _CurrentIdentifierBuffer->Append("array");
        }

        _CurrentValueBuffer->AppendByte(currentChar);

        // Handle reading the method arguments.
        if (!recording && !isAssignment && currentChar == '(') {
            _ParseMethodArgs();
            return;
        }
    }

    sympl_assert(false, "Missing ';' detected for object value during parsing!");
}

void ScriptParser::_ParseMethodArgs()
{
    char currentChar = '\0';
    char nextChar = '\0';
    int parethNestLevel = 1; // We're initially inside of a parameter.
    bool recording = false;
    _ScanMode = ParserScanMode::Type;

    auto buffer = _Reader->GetBuffer();
    SharedPtr<StringBuffer> methodArg = mem_alloc_ref(StringBuffer);
    _MethodArgs.clear();

    while (_CharLocation < buffer->Length()) {
        currentChar = buffer->Get(_CharLocation);
        nextChar = buffer->Get(_CharLocation + 1);
        _CharLocation++;

        // Check if we're starting a string.
        if (currentChar == '%' && buffer->PeekSearch(SYMPL_STRING_TOKEN, _CharLocation - 1)) {
            recording = !recording;
            _CurrentValueBuffer->Append(SYMPL_STRING_TOKEN);
            _CharLocation = _CharLocation + strlen(SYMPL_STRING_TOKEN) - 1;
            methodArg->Append(SYMPL_STRING_TOKEN);
            continue;
        }

        _CurrentValueBuffer->AppendByte(currentChar);

        if (!recording) {
            if (currentChar == '(') {
                parethNestLevel++;
            }
            if (currentChar == ')') {
                parethNestLevel--;
            }

            if (currentChar == ',' || nextChar == ';' || nextChar == '{') {
                _MethodArgs.emplace_back(methodArg->CStr());
                methodArg->Clear();
            } else if (currentChar != ')' || parethNestLevel != 0) {
                if (currentChar == '#' && methodArg->Length() != 0) {
                    methodArg->AppendByte(currentChar);
                } else if (currentChar != '#') {
                    methodArg->AppendByte(currentChar);
                }
            }
        } else {
            methodArg->AppendByte(currentChar);
        }

        // Method is being called.
        if (nextChar == ';') {
            _CharLocation++; // move forward so we can start parsing the next object.
            return;
        }

        // Method is being being defined.
        if (nextChar == '{') {
            return;
        }
    }

    sympl_assert(false, "Missing ')' detected for object method arguments!");
}

void ScriptParser::_BuildObject()
{
    // Check if our object exists.
    auto scriptObject = _FindObject(_CurrentObjectBuffer->CStr());

    // Determine the object type.
    if (scriptObject->IsEmpty()) {
        // Build our object.
        auto parentObject = _CurrentScopeObject.IsValid() ? _CurrentScopeObject.Ptr() : nullptr;

        if (_CurrentIdentifierBuffer->Equals("var")) {
            _CurrentObject = ScriptVMInstance.CreateObjectAndInitialize<ScriptObject>(_CurrentObjectBuffer->CStr(), parentObject);
        } else if (_CurrentIdentifierBuffer->Equals("array")) {
            _CurrentObject = ScriptVMInstance.CreateObjectAndInitialize<ScriptArray>(_CurrentObjectBuffer->CStr(), parentObject);
        } else if (_CurrentIdentifierBuffer->Equals("func")) {
            _CurrentObject = ScriptVMInstance.CreateObjectAndInitialize<ScriptMethod>(_CurrentObjectBuffer->CStr(), parentObject);
        } else if (_CurrentIdentifierBuffer->Equals("class")) {
            _CurrentObject = ScriptVMInstance.CreateObjectAndInitialize<ScriptObject>(_CurrentObjectBuffer->CStr(), parentObject);
        } else {
            sympl_assert(false, "Failed to identify the object type");
        }

    } else {
        _CurrentObject = scriptObject;
    }

    // Check if our object is part of a 'scoped' method (if, else, while, etc).
    if (scriptObject->IsMethod()) {
        // This will be a method reference (pointer to a method).
        auto parentObject = _CurrentScopeObject.IsValid() ? _CurrentScopeObject.Ptr() : nullptr;

        // Needs to use a unique name.
        auto uniqueStr = StringHelper::GenerateRandomStr(5);
        _CurrentObjectBuffer->Append(uniqueStr);

        _CurrentObject = ScriptVMInstance.CreateObjectAndInitialize<ScriptMethod>(
                _CurrentObjectBuffer->CStr(),
                parentObject
        );

        // Set our reference.
        _CurrentObject->SetReference(scriptObject);
    }

    // Add our arguments.
    if (_CurrentObject->IsMethod() && !_MethodArgs.empty()) {
        for (auto& arg : _MethodArgs) {
            to_method(_CurrentObject.Ptr())->AddArg(arg.c_str());
        }
    }

    // Add object processing to our interpreter for the main scope.
    if (!_CurrentScopeObject.IsValid()) {
        if (_CurrentObject->IsMethod() && !_CurrentObject->IsReference()) {
            return;
        }
        _Interpreter->AddCommand(
                ScriptVMInstance.GetGlobalObject()->GetObjectAddress(),
                _CurrentObject.Ptr(),
                _CurrentValueBuffer->CStr()
        );
    } else {
        auto scopeParent = _CurrentScopeObject->GetParent().Ptr();
        if (!scopeParent->IsMethod()) {
            return;
        }
        to_method(scopeParent)->AddCallStatement(
                _CurrentObject.Ptr(),
                _CurrentValueBuffer->CStr()
        );
    }
}

void ScriptParser::_OpenScope()
{
    // Check if we need to exit out of the argument building.
    _CurrentScopeObject = _CurrentObject->FindChildByName(SYMPL_SCOPE_NAME, false);
    if (_CurrentScopeObject->IsEmpty()) {
        _CurrentScopeObject = ScriptVMInstance.CreateObjectAndInitialize<ScriptObject>(
                SYMPL_SCOPE_NAME,
                _CurrentObject.Ptr()
        );
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
    if (_CurrentObject->GetParent().IsValid() && _CurrentObject->GetParent().Ptr() != ScriptVMInstance.GetGlobalObject()) {
        _CurrentScopeObject = _CurrentObject->GetParent().Ptr();
    }

    _ScanMode = ParserScanMode::Type;
}

ScriptObject* ScriptParser::_FindObject(const char* objectName)
{
    ScriptObject* output = &ScriptObject::Empty;

    if (_CurrentObject.IsValid() && _CurrentObject->GetName() == objectName) {
        return _CurrentObject.Ptr();
    }

    if (_CurrentObject.IsValid()) {
        if (_CurrentScopeObject.IsValid()) {
            output = _CurrentScopeObject->FindChildByName(objectName);
        } else {
            output = _CurrentObject->FindChildByName(objectName);
        }
    }

    // If this is still empty, and we don't have a parent, check the global object.
    if (output->IsEmpty() && !_CurrentScopeObject.IsValid()) {
        output = ScriptVMInstance.GetGlobalObject()->FindChildByName(objectName, false);
    }

    return output;
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
    mem_free_ref(StringBuffer, _CurrentIdentifierBuffer);
    mem_free_ref(StringBuffer, _CurrentObjectBuffer);
    mem_free_ref(StringBuffer, _CurrentValueBuffer);

    return true;
}
