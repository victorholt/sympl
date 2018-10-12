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
#include <sympl/script/script_method.h>
sympl_namespaces

ScriptMethod::ScriptMethod()
{
    __Construct();
}

void ScriptMethod::__Construct()
{
    _Type = ScriptObjectType::Method;
}

Variant ScriptMethod::Evaluate(ScriptMethodArgs args, ScriptObject* caller)
{
    if (IsReference()) {
        return to_method(GetReference())->Evaluate(args, this);
    }

    ProcessCallStatements();

    return _Value;
}

Variant ScriptMethod::Evaluate(ScriptObject* caller)
{
    if (IsReference()) {
        return to_method(GetReference())->Evaluate(this);
    }

    ProcessCallStatements();

    return _Value;
}

void ScriptMethod::ProcessCallStatements()
{
    for (auto call : _CallStatements) {
        call.Resolver = mem_alloc_ref(StatementResolver);
        call.Resolver->Resolve(call.StatementStr, call.ObjectRef.Ptr());
        call.Resolver.Release();
    }
    _Value = Variant::Empty;
}

void ScriptMethod::AddCallStatement(ScriptObject* ref, const char* str)
{
    MethodCallStatement stmt;
    stmt.ObjectRef = ref;
    stmt.StatementStr = str;
    _CallStatements.emplace_back(stmt);
}

void ScriptMethod::AddArg(const char* name)
{
    _MethodArgs.emplace_back(name);
}
