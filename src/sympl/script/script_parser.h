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
#include <sympl/core/string_buffer.h>
#include <sympl/core/object.h>
#include <sympl/core/shared_ptr.h>
#include <sympl/script/script_reader.h>
#include <sympl/script/script_token.h>
#include <sympl/script/script_object.h>

sympl_nsstart

class ScriptReader;
class Interpreter;

class SYMPL_API ScriptParser : public Object
{
    SYMPL_OBJECT(ScriptParser, Object);

private:
    enum class ParserScanMode {
        Type = 0,
        VarName,
        Value,
        MethodArgs
    };

    /// Symbol token checker.
    ScriptToken* _Symbol;

    /// Reference to the interpreter.
    Interpreter* _Interpreter;

    /// Current object we're building.
    WeakPtr<ScriptObject> _CurrentObject;

    /// Current scope object.
    WeakPtr<ScriptObject> _CurrentScopeObject;

    /// Current operator for the object.
    char _CurrentOperator[3];

    /// Current identifier string buffer.
    StringBuffer* _CurrentIdentifierBuffer;

    /// Current object string buffer.
    StringBuffer* _CurrentObjectBuffer;

    /// Current value string buffer.
    StringBuffer* _CurrentValueBuffer;

    /// Current script reader object.
    ScriptReader* _Reader;

    /// Determines what the parser is currently looking for.
    ParserScanMode _ScanMode = ParserScanMode::Type;

    /// Flag for whether or not the parser is recording a string.
    bool _RecordingString = false;

    /// Flag for if we should store a virtual command.
    bool _IsVirtualCommand = false;

    /// Current line we're parsing.
    size_t _CurrentLine = 0;

    /// Current character location
    size_t _CharLocation = 0;

    //! Parses the current buffer.
    void _ParseBuffer(ScriptReader* reader);

    //! Builds the current object.
    void _BuildObject();

    //! Build out the method arguments with the current buffer.
    void _BuildMethodArgs();

    //! Scan a potential array statement.
    void _ScanArrayStatement();

    //! Updates the object's value.
    void _UpdateObjectValue();

    //! Registers an objects constructor/destructor method.
    void _RegisterObjectConstructs();

    //! Attempts to update the scan mode.
    void _UpdateScanMode();

    //! Opens a new scope based on the current object.
    void _OpenScope();

    //! Closes the current scope.
    void _CloseScope();

    //! Checks if an object exists based on the current scope.
    //! \return bool
    bool _ObjectExists(const char* objectName);

    //! Checks if an object exists based on the current scope.
    //! \param objectName
    //! \return ScriptObject
    ScriptObject* _FindObject(const char* objectName);

    //! Clears the buffers.
    void _ClearBuffers();

public:
    //! Constructor.
    ScriptParser();

    //! Called in place of the constructor.
    void __Construct() override;

    //! Handles parsing.
    //! \param interpreter
    void Parse(Interpreter* interpreter);

    //! Releases the object.
    bool Release() override;
};

sympl_nsend
