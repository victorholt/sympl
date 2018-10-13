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
#include <sympl/script/statement_resolver.h>

sympl_nsstart

class StatementResolver;

struct MethodCallStatement
{
    WeakPtr<ScriptObject> ObjectRef;
    SharedPtr<StatementResolver> Resolver;
    std::string StatementStr;
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
    // Statements called by the method.
    std::vector<MethodCallStatement> _CallStatements;

    // Method argument strings.
    std::vector<std::string> _MethodArgs;

    // Scope for the method.
    SharedPtr<ScriptObject> _Scope;

public:
    //! Constructor.
    ScriptMethod();

    //! Called in place of the constructor.
    void __Construct() override;

    //! Evaluates and returns the results of the object.
    //! \param args
    //! \return
    virtual Variant Evaluate(ScriptMethodArgs args, ScriptObject* caller = nullptr);

    //! Evaluates and returns the results of the object.
    //! \return
    virtual Variant Evaluate(ScriptObject* caller = nullptr);

    //! Copy over given arguments.
    //! \param args
    void CopyArgs(const ScriptMethodArgs args);

    //! Processes the arguments and returns them through the output.
    //! \param output
    void ProcessArgs(ScriptMethodArgs output);

    //! Process the call statements.
    Variant ProcessCallStatements();

    //! Adds a call statement to the method.
    //! \param ref
    //! \param str
    void AddCallStatement(ScriptObject* ref, const char* str);

    //! Add an argument to the method.
    //! \param name
    void AddArg(const char* name);

    //! Returns the scope for the method.
    //! \return ScriptObject
    ScriptObject* GetScope();
};

#define to_method(obj) dynamic_cast<ScriptMethod*>(obj)

sympl_nsend
