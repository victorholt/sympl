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
#include <sympl/script/script_common.h>

sympl_nsstart

class SYMPL_API EvalResolver : public Object
{
    SYMPL_OBJECT(EvalResolver, Object);

public:
    //! Constructor.
    EvalResolver() = default;

    //! Called in place of the constructor.
    void __Construct() override {}

    //! Returns a value from evaluating a given script object.
    //! \param stmtStr
    //! \param scriptObject
    //! \return
    Variant GetEvalFromStatementBuffer(const char* stmtStr, ScriptObject* scriptObject);

    //! Returns whether or not a given script object evaluates to true.
    //! \param stmtStr
    //! \param scriptObject
    //! \return
    bool IsStatementBufferTrue(const char* stmtStr, ScriptObject* scriptObject);
};

sympl_nsend
