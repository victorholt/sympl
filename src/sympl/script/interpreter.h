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
#include <sympl/core/sympl_object.h>
#include <sympl/core/shared_ref.h>
#include <sympl/script/script_common.h>
#include <sympl/script/script_object.h>
#include <sympl/io/script_reader.h>

sympl_nsstart

class SymplVM;

struct InterpretCommandEntry
{
    SharedRef<ScriptObject>       ObjectRef;
    SharedRef<ScriptStatement>    Statement;
};

class SYMPL_API Interpreter : public Object
{
    SYMPL_OBJECT(Interpreter, Object);

private:
    /// Command list for the program.
    std::vector<InterpretCommandEntry> _CommandList;

    /// Reference to the script reader.
    SharedRef<ScriptReader> _Reader;

    //! Sets the reader for the interpreter.
    //! \param reader
    void _SetReader(ScriptReader* reader);

    //! Parses the file.
    void _Parse();

public:
    //! Constructor.
    Interpreter();

    //! Destructor.
    ~Interpreter() override;

    //! Returns the script reader.
    inline ScriptReader* GetReader() const {
        return _Reader.Ptr();
    }

    // Enable the SymplVM as a friend.
    friend SymplVM;
};

sympl_nsend
