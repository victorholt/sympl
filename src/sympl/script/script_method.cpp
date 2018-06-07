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
#include <sympl/script/script_statement.h>
#include <sympl/core/string_buffer.h>
#include <sympl/script/script_vm.h>
#include <sympl/script/statement_resolver.h>

#include <fmt/format.h>
sympl_namespaces

ScriptMethod::ScriptMethod()
{
    __Construct();
}

void ScriptMethod::__Construct()
{
    _Type = ScriptObjectType::Method;
    _ReturnType = MethodReturnType::Object;
    _ArgString = mem_alloc(StringBuffer);
}

void ScriptMethod::_Initialize(const char* name, const char* path, ScriptObject* parent)
{
    ScriptObject::_Initialize(name, path, parent);

    // Create the scope.
    _Scope = ScriptVMInstance->CreateObject(SYMPL_SCOPE_NAME, ScriptObjectType::Object, this);
    _Scope->GetContext()->SetParentContext(_Context.Ptr());
}

Variant ScriptMethod::Evaluate(const Urho3D::PODVector<Variant>& args)
{
    _Exit = false;

    _CopyArgs(args);
    _ProcessArgStatements();
    _ProcessCallStatements();

    return _Value;
}

Variant ScriptMethod::Evaluate()
{
    Urho3D::PODVector<Variant> args;
    return Evaluate(args);
}

void ScriptMethod::_CopyArgs(const Urho3D::PODVector<Variant>& args)
{
    int argIndex = 0;
    for (auto argIt : _Args) {
        // Ensure we have args.
        if (argIndex >= args.Size()) {
            break;
        }

        Variant argValue = args[argIndex];

        auto argObj = GetScope()->TraverseUpFindChildByName(argIt->GetName().c_str(), false);
        assert(!argObj->IsEmpty() && "Invalid argument given for method");

        argObj->SetValue(argValue);

        argIndex++;
    }
}

void ScriptMethod::_ProcessArgStatements()
{
    // String arguments will need to have statements
    // to execute their value.
    for (auto argIt : _Args) {
        Variant value = argIt->GetValue();

        if (value.GetType() == VariantType::StringBuffer) {
            SharedPtr<StatementResolver> resolver = mem_alloc(StatementResolver);

            Variant argValue = argIt->GetValue();
            argValue = resolver->Resolve(argValue.GetStringBuffer()->CStr(), argIt);

            // If this is a string we will need to replace the token.
            if (argValue.GetType() == VariantType::StringBuffer) {
                argValue.GetStringBuffer()->Replace(SYMPL_STRING_TOKEN, "");
            }

            argIt->SetValue(argValue);
        }
    }
}

void ScriptMethod::_ProcessCallStatements()
{
    for (auto entryIt : _CallStatements) {
        if (_Exit) return;

        SharedPtr<StatementResolver> resolver = mem_alloc(StatementResolver);
        entryIt->Variable->GetContext()->SetCallerContext(GetScope()->GetContext());

        auto val = resolver->Resolve(entryIt->StatementStr->CStr(), entryIt->Variable.Ptr());
        if (!val.IsEmpty()) {
            _Value = val;
            entryIt->Variable->SetValue(_Value);
        }

        // Check if we're attempting to return out of the method.
        if (entryIt->Variable->GetName() == "return") {
            Exit();
            return;
        }
    }
}

bool ScriptMethod::_CheckReturnType(const Variant& value)
{
    VariantType type = value.GetType();
    if (type == VariantType::Int && (_ReturnType != MethodReturnType::Int && _ReturnType != MethodReturnType::String)) {
        return false;
    }
    if (type == VariantType::Float && (_ReturnType != MethodReturnType::Float && _ReturnType != MethodReturnType::String)) {
        return false;
    }
    if (type == VariantType::StringBuffer && (_ReturnType != MethodReturnType::String)) {
        return false;
    }
    if (type == VariantType::Bool && (_ReturnType != MethodReturnType::Bool && _ReturnType != MethodReturnType::String)) {
        return false;
    }
    if (type == VariantType::Object && (_ReturnType != MethodReturnType::Object && _ReturnType != MethodReturnType::String)) {
        return false;
    }
    return true;
}

void ScriptMethod::AddArg(ScriptObject* arg)
{
    // Check if we already have this object.
    for (auto argIt : _Args) {
        if (argIt->GetName() == arg->GetName()) {
            return;
        }
    }

    arg->SetMeta("is_method_arg", true);
    _Args.Push(arg);

    // Add arg as a variable in the context.
    GetScope()->GetContext()->AddVar(arg);
}

void ScriptMethod::AddStatement(ScriptObject* variable, const char* stmtStr)
{
    auto callStatement = alloc_bytes(MethodCallStatement);
    callStatement->Variable = variable;
    callStatement->StatementStr = mem_alloc(StringBuffer);
    callStatement->StatementStr->Append(stmtStr);

    _CallStatements.Push(callStatement);
}

ScriptObject* ScriptMethod::Clone(ScriptObject* parent, bool uniqueName)
{
    auto clone = ScriptObject::Clone(parent, uniqueName);
    to_method(clone)->SetImmediate(_IsImmediate);
    to_method(clone)->SetReturnType(_ReturnType);
    to_method(clone)->SetIgnoreReturnTypeCheck(_IgnoreReturnTypeCheck);

    // Ensure everything in the scope has been cloned!
    for (auto entryIt : GetScope()->GetChildren()) {
        auto child = to_method(clone)->GetScope()->FindChildByName(entryIt->GetName().c_str(), false);
        if (child->IsEmpty()) {
            entryIt->Clone(to_method(clone)->GetScope(), false);
        }
    }

    // Add the arguments.
    for (auto entryIt : _Args) {
        auto argObj = to_method(clone)->GetScope()->FindChildByName(entryIt->GetName().c_str(), false);
        if (!argObj->IsEmpty()) {
            to_method(clone)->AddArg(argObj);
        }
    }

    // Add the statements.
    for (auto entryIt : _CallStatements) {
        auto callObj = to_method(clone)->GetScope()->TraverseUpFindChildByName(entryIt->Variable->GetName().c_str(), false);
        if (!IsNullObject(callObj) && !callObj->IsEmpty()) {
            to_method(clone)->AddStatement(callObj, entryIt->StatementStr->CStr());
        }
    }

    return clone;
}

ScriptObject* ScriptMethod::GetScope()
{
    return _Scope.Ptr();
}

ScriptObject* ScriptMethod::GetScopeParent()
{
    auto scope = GetScope();
    auto parent = scope->GetParent();
    return (parent.IsValid() ? parent.Ptr() : &ScriptObject::Empty);
}

void ScriptMethod::Exit() {
    _Exit = true;

    if (!GetContext()->GetCallerContext()->IsEmpty()) {
        auto obj = GetContext()->GetCallerContext()->GetScriptObject();
        if (!_Value.IsEmpty()) obj->SetValue(_Value);

        if (obj->GetType() == ScriptObjectType::Method) {
            to_method(obj)->Exit();
        } else {
            auto parent = obj->GetParent();
            if (parent.IsValid() && parent->GetType() == ScriptObjectType::Method) {
                if (!_Value.IsEmpty()) parent->SetValue(_Value);
                to_method(parent.Ptr())->Exit();
            }
        }
    }
}

bool ScriptMethod::Release()
{
    _Value.Clear();

    for (auto entryIt : _CallStatements) {
        free_bytes(entryIt);
    }
    _CallStatements.Clear();
    _Args.Clear();

    return true;
}
