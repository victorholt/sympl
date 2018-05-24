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
#include <sympl/script/sympl_vm.h>

#include <fmt/format.h>
sympl_namespaces

ScriptMethod::ScriptMethod()
{
    _Type = ScriptObjectType::Method;
    _ReturnType = MethodReturnType::Object;
    _ArgString = alloc_ref(StringBuffer);
    _ArgString->Resize(512);
}

void ScriptMethod::_Initialize(const char* name, const char* path, ScriptObject* parent)
{
    ScriptObject::_Initialize(name, path, parent);

    // Create the scope.
    _Scope = SymplVMInstance->CreateObject(SYMPL_SCOPE_NAME, ScriptObjectType::Object, this);
}

Variant ScriptMethod::Evaluate(const std::vector<Variant>& args)
{
    _CopyArgs(args);
    _ProcessArgStatements();
    _ProcessCallStatements();

    return _Value;
}

Variant ScriptMethod::Evaluate()
{
    std::vector<Variant> args;
    return Evaluate(args);
}

void ScriptMethod::_CopyArgs(const std::vector<Variant>& args)
{
    int argIndex = 0;
    for (auto argIt : _Args) {
        // Ensure we have args.
        if (argIndex >= args.size()) {
            break;
        }

        Variant argValue = args[argIndex];

        // std::cout << "LOOKING FOR ARG: " << argIt->GetName() << " with value " << argValue.AsString() << std::endl;

        auto argObj = GetScope()->TraverseUpFindChildByName(argIt->GetName().c_str(), false);
        assert(!argObj->IsEmpty() && "Invalid argument given for method");

        // std::cout << "COPY ARG VALUE: " << argObj->GetPath() << " = " << argValue.AsString() << std::endl;
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
            SharedRef<ScriptStatement> stat = alloc_ref(ScriptStatement);

            Variant argValue = argIt->GetValue();
            stat->Build(argIt.Ptr(), argValue.GetStringBuffer());

            argValue = stat.Ptr()->Evaluate();
            argIt->SetValue(argValue);
        }
    }
}

void ScriptMethod::_ProcessCallStatements()
{
    for (auto entryIt : _CallStatements) {
        if (_Exit) {
            return;
        }

        // std::cout << entryIt->Variable->GetName() << " PROCESS CALL FROM: " << GetContext()->GetCurrentScope()->GetPath() << std::endl;
        entryIt->Statement->Build(entryIt->Variable.Ptr());

        _Value = entryIt->Statement->Evaluate();
        entryIt->Variable->SetValue(_Value);

        // Check if we're attempting to return out of the method.
        if (entryIt->Variable->GetName() == "return") {
            std::cout << " RETURNING VALUE: " << _Value.AsString() << std::endl;
            GetContext()->SetReturnValue(_Value);
            Exit();
            return;
        }
    }
}

bool ScriptMethod::_CheckReturnType(const Variant& value)
{
    Variant v = value;
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
    if (type == VariantType::ScriptObject && (_ReturnType != MethodReturnType::Object && _ReturnType != MethodReturnType::String)) {
        return false;
    }
    return true;
}

void ScriptMethod::AddArg(ScriptObject* arg)
{
    arg->SetMeta("is_method_arg", true);
    _Args.push_back(arg);
}

void ScriptMethod::AddStatement(ScriptObject* variable, ScriptStatement* stat)
{
    MethodCallStatement* callStatement = new MethodCallStatement();
    callStatement->Variable = variable;
    callStatement->Statement = stat;

    _CallStatements.push_back(callStatement);
}

ScriptObject* ScriptMethod::Clone(ScriptObject* parent, bool uniqueName)
{
    auto clone = ScriptObject::Clone(parent, uniqueName);
    to_method(clone)->SetImmediate(_IsImmediate);
    to_method(clone)->SetReturnType(_ReturnType);
    to_method(clone)->SetIgnoreReturnTypeCheck(_IgnoreReturnTypeCheck);

    // Ensure everything in the scope has been cloned!
    for (auto entryIt : GetScope()->GetChildren()) {
        auto child = to_method(clone)->GetScope()->FindChildByName(entryIt.second->GetName().c_str(), false);
        if (child->IsEmpty()) {
            entryIt.second->Clone(to_method(clone)->GetScope(), false);
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
            to_method(clone)->AddStatement(callObj, entryIt->Statement->Clone(to_method(clone)->GetScope()));
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
}

void ScriptMethod::Release()
{
    _Value.Clear();

    for (auto entryIt : _CallStatements) {
        delete entryIt;
    }
    _CallStatements.clear();
    _Args.clear();

    for (auto childIt : _Children) {
        childIt.second->Release();
    }
    _Children.clear();

    ScriptObject::Release();
}
