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

#include "../core/sympl_pch.h"
#include "../core/string_buffer.h"
#include "../core/sympl_object.h"
#include "../core/shared_ref.h"

#include "../script/script_object.h"

#include "script_reader.h"

sympl_nsstart

class SYMPL_API ScriptCompiler : public Object
{
private:
    /// Root object we're compiling.
    SharedRef<ScriptObject> _RootObject;

    /// Current identifier.
    char _CurrentIdentifier[6];

    /// Current operator for the object.
    char _CurrentOperator[3];

    /// Current object string buffer.
    StringBuffer* _CurrentObjectBuffer;

    /// Current value string buffer.
    StringBuffer* _CurrentValueBuffer;

    //! Parses the current buffer.
    void _ParseBuffer(ScriptReader* reader);

    //! Builds the current object.
    void _BuildObject();

    //! Clears the buffers.
    void _ClearBuffers();

public:
    //! Constructor.
    ScriptCompiler();

    //! Destructor.
    virtual ~ScriptCompiler();

    //! Handles compiling a file.
    //! \param filePath
    void CompileFile(const char* filePath);

    //! Handles parsing a string.
    //! \param str
    void CompileString(const char* str);
};

sympl_nsend
