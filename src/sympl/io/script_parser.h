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
#include <sympl/core/sympl_object.h>
#include <sympl/core/shared_ref.h>
#include <sympl/io/script_reader.h>
#include <sympl/io/script_symbol.h>
#include <sympl/script/script_object.h>
#include <sympl/script/script_statement.h>

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
    ScriptSymbol _Symbol;

    /// Reference to the interpreter.
    Interpreter* _Interpreter;

    /// Current object we're building.
    SharedRef<ScriptObject> _CurrentObject;

    /// Current scope object.
    SharedRef<ScriptObject> _CurrentScopeObject;

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

    //! Updates the object's value.
    void _UpdateObjectValue();

    //! Attempts to update the scan mode.
    void _UpdateScanMode();

    //! Opens a new scope based on the current object.
    void _OpenScope();

    //! Closes the current scope.
    void _CloseScope();

    //! Converts a symbol to a statement operator.
    //! \param symbol
    StatementOperator _SymbolToOp(const std::string& symbol);

    //! Checks if an object exists based on the current scope.
    //! \return bool
    bool _ObjectExists(const char* objectName);

    //! Clears the buffers.
    void _ClearBuffers();

public:
    //! Constructor.
    ScriptParser();

    //! Destructor.
    ~ScriptParser() override;

    //! Handles parsing.
    //! \param interpreter
    void Parse(Interpreter* interpreter);
};

sympl_nsend
