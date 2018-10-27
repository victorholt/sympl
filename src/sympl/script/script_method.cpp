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
#include <sympl/script/script_vm.h>
#include <sympl/script/resolvers/eval_resolver.h>
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
    // Since we're taking in arguments, we'll copy them over.
    CopyArgs(args);

    if (IsReference()) {
        return to_method(GetReference())->Evaluate(args, this);
    }

    return ProcessCallStatements();
}

Variant ScriptMethod::Evaluate(ScriptObject* caller)
{
    ScriptMethodArgList argList;
    ProcessArgs(argList);

    if (IsReference()) {
        return to_method(GetReference())->Evaluate(argList, this);
    }

    return ProcessCallStatements();
}

void ScriptMethod::CopyArgs(const ScriptMethodArgs args)
{
    // Ensure we've created the arguments.
    sympl_assert(_MethodArgs.size() >= args.size(), "Too many arguments given in method!");

    for (size_t i = 0; i < args.size(); i++) {
        const auto& argName = _MethodArgs[i];
        const auto& arg = args[i];

        auto argObject = GetScope()->FindChildByName(argName);
        if (argObject->IsEmpty()) {
            argObject = ScriptVMInstance.CreateObjectAndInitialize<ScriptObject>(
                    argName,
                    GetScope()
            );
        }

        argObject->SetValue(arg);
    }
}

void ScriptMethod::ProcessArgs(ScriptMethodArgs output)
{
    if (_MethodArgs.empty() || _MethodArgs[0].empty()) {
        return;
    }

    SharedPtr<StatementResolver> resolver = mem_alloc_ref(StatementResolver);
    for (const auto& arg : _MethodArgs) {
        resolver->ClearStatementEntries();
        Variant vArg = resolver->Resolve(arg, this);
        if (vArg.GetType() == VariantType::Object) {
            output.emplace_back(to_script_object(vArg)->GetValue());
        } else {
            output.emplace_back(vArg);
        }
    }
}

Variant ScriptMethod::ProcessCallStatements()
{
    Variant ret = Variant::Empty;
    for (auto& call : _CallStatements) {
        // Check if we're returning a value.
        if (call.ObjectRef->GetName() == "return") {
            EvalResolver evalResolver;
            Variant eval = evalResolver.GetEvalFromStatementBuffer(call.StatementStr.c_str(), call.ObjectRef.Ptr());

            // TODO: Add return type to methods to skip this step.
            // We need to figure out whether or value is a string, number, or object.

            if (eval.GetType() == VariantType::Object) {
                ret = to_script_object(eval)->GetValue();

                if (ret.GetType() == VariantType::StringBuffer) {
                    StringBuffer* buffer = ret.GetStringBuffer();
                    buffer->Prepend(SYMPL_STRING_TOKEN);
                    buffer->Append(SYMPL_STRING_TOKEN);
                }
            } else if (eval.GetType() == VariantType::StringBuffer) {
                StringBuffer* buffer = eval.GetStringBuffer();
                buffer->Prepend(SYMPL_STRING_TOKEN);
                buffer->Append(SYMPL_STRING_TOKEN);
                ret = buffer;
            } else {
                ret = eval;
            }

            break;
        }

        if (call.ObjectRef->IsMethod()) {
            ret = to_method(call.ObjectRef.Ptr())->Evaluate(this);
        } else {
            if (!call.Resolver.IsValid()) {
                call.Resolver = mem_alloc_ref(StatementResolver);
                call.Resolver->SetCache(true);
            }
            ret = call.Resolver->Resolve(call.StatementStr, call.ObjectRef.Ptr());
//            call.Resolver.Release();

            call.ObjectRef->SetValue(ret);
        }
    }
    return ret;
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

ScriptObject* ScriptMethod::GetScope()
{
    if (!_Scope.IsValid()) {
        _Scope = FindChildByName(SYMPL_SCOPE_NAME, false);
        if (_Scope->IsEmpty()) {
            _Scope = ScriptVMInstance.CreateObjectAndInitialize<ScriptObject>(
                    SYMPL_SCOPE_NAME,
                    this
            );
        }
    }
    return _Scope.Ptr();
}
