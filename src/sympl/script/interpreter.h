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
#include <sympl/core/object.h>
#include <sympl/core/shared_ptr.h>
#include <sympl/script/script_common.h>
#include <sympl/script/script_object.h>
#include <sympl/script/script_reader.h>
#include <sympl/script/script_parser.h>

sympl_nsstart

class ScriptVM;

struct InterpretCommandEntry
{
    WeakPtr<ScriptObject>         ObjectRef;
    SharedPtr<StringBuffer>       StatementStr;
    SharedPtr<StringBuffer>       VirtualObjectRef;
};

class SYMPL_API Interpreter : public Object
{
    SYMPL_OBJECT(Interpreter, Object);

private:
    /// Command list for the program based on an object address.
    std::unordered_map< MemAddressType, std::vector<InterpretCommandEntry> > _CommandList;

    /// Reference to the script parser.
    SharedPtr<ScriptParser> _Parser;

    /// Reference to the script reader.
    SharedPtr<ScriptReader> _Reader;

    //! Sets the reader for the interpreter.
    //! \param reader
    void _SetReader(ScriptReader* reader);

    //! Parses the file.
    void _Parse();

public:
    //! Constructor.
    Interpreter();

    //! Called in place of the constructor.
    void __Construct() override;

    //! Attempts to run the program.
    //! \return scopeObjectAddress
    //! \return bool
    bool Run(MemAddressType scopeObjectAddress = -1);

    //! Adds a command to the interpreter.
    //! \param scopeObjectAddress
    //! \param objectRef
    //! \param stmtStr
    void AddCommand(MemAddressType scopeObjectAddress, ScriptObject* objectRef, const char* stmtStr);

    //! Adds a virtual command to the interpreter.
    //! \param command
    //! \param stmtStr
    void AddVirtualCommand(MemAddressType scopeObjectAddress, const char* command, const char* stmtStr);

    //! Releases the object.
    bool Release() override;

    //! Returns the script reader.
    inline ScriptReader* GetReader() const {
        return _Reader.Ptr();
    }

    // Enable the SymplVM as a friend.
    friend ScriptVM;
};

sympl_nsend
