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
        _ParseLineNumber();

        switch ((int)_ScanMode) {
            case (int)ParserScanMode::Type: {
                _ParseType();
                _ScanMode = ParserScanMode::VarName;
            }
            break;
            case (int)ParserScanMode::VarName: {
                _ParseName();
                _ScanMode = ParserScanMode::Value;
            }
            break;
            case (int)ParserScanMode::Value: {
                _ParseValue();
                _ScanMode = ParserScanMode::Type;
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
                return;
            }
        }
        // End grabbing the line number.
    }

    sympl_assert(false, "Failed to parse line number!");
}

void ScriptParser::_ParseType()
{
    int _CurrentLine = 0;
    unsigned bufferIndex = 0;
    char currentChar = '\0';
    char nextChar = '\0';
    bool valueMode = false;
    bool inArray = false;
    bool grabbingLineNumber = false;

    while (_CharLocation < _Reader->GetBuffer()->Length()) {
        currentChar = _Reader->GetBuffer()->Get(_CharLocation);
        nextChar = _Reader->GetBuffer()->Get(_CharLocation + 1);
        _CharLocation++;

        // Store the value.
        if (currentChar == '#') {

        }
    }

    sympl_assert(false, "Missing type for declared variable!");
}

void ScriptParser::_ParseName()
{

}

void ScriptParser::_ParseValue()
{

}

void ScriptParser::_BuildObject()
{

}

void ScriptParser::_OpenScope()
{
    // Check if we need to exit out of the argument building.
    _CurrentScopeObject = _CurrentObject->FindChildByName(SYMPL_SCOPE_NAME);
    if (_CurrentScopeObject->IsEmpty()) {
        _CurrentScopeObject = ScriptVMInstance.CreateObjectAndInitialize(SYMPL_SCOPE_NAME, ScriptObjectType::Object, _CurrentObject.Ptr());
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

ScriptObject* ScriptParser::_FindObject(const char* objectName)
{
    ScriptObject* output = &ScriptObject::Empty;
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
