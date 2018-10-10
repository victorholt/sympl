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
#include <sympl/core/shared_ptr.h>
#include <sympl/core/weak_ptr.h>
#include <sympl/core/string_buffer.h>
#include <sympl/script/script_object.h>

sympl_nsstart

class StatementResolver;

struct MethodCallStatement
{
    WeakPtr<ScriptObject> Variable;
    SharedPtr<StringBuffer> VirtualObjectStr;
    SharedPtr<StringBuffer> StatementStr;
    SharedPtr<StatementResolver> Resolver;
};

struct MethodArgCacheValue
{
    Variant                 ArgValue;

    // Need to know how many characters to advance
    // when parsing the statement string.
    size_t                  ArgStringLength;
};

struct MethodArgCache
{
    char ArgString[256];
    std::vector<MethodArgCacheValue> Args;
};

class SYMPL_API ScriptMethod : public ScriptObject
{
    SYMPL_OBJECT(ScriptMethod, ScriptObject);

protected:


public:
    //! Constructor.
    ScriptMethod();

    //! Called in place of the constructor.
    void __Construct() override;

    //! Evaluates and returns the results of the object.
    //! \param args
    //! \return
    Variant Evaluate(ScriptMethodArgs args);

    //! Evaluates and returns the results of the object.
    //! \return
    Variant Evaluate();

    //! Add an argument to the method.
    //! \param name
    void AddArg(const char* name);
};

sympl_nsend
