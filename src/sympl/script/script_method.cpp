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

ScriptMethod::~ScriptMethod()
{
    for (auto entryIt : _CallStatements) {
        delete entryIt;
    }
    _CallStatements.clear();
    _Args.clear();
}

Variant ScriptMethod::Evaluate(const std::vector<Variant>& args)
{
    for (auto entryIt : _CallStatements) {
        entryIt->Variable->SetValue(entryIt->Statement.Ptr());
    }

    // Our return value.
    Variant retValue;

    // Clone the method to avoid reference issues.
    auto clone = Clone(this, true);

    assert((!clone->IsEmpty()) && "Unable to evaluate method!");
    assert((clone->GetType() == ScriptObjectType::Method) && "Invalid object called as a method!");
    assert((!to_method(clone)->GetScope()->IsEmpty()) && "Method has an empty scope object!");

    // Update the arguments.
    to_method(clone)->_CopyArgs(args);

    // Go through the arguments and build their statements.
    to_method(clone)->_ProcessArgStatements();

    // Go through all of the call statements.
    to_method(clone)->_ProcessCallStatements();

    // Check if we have a return variable.
    if (_ReturnType != MethodReturnType::Void) {
        auto retObject = to_method(clone)->GetScope()->FindChildByName("return");
        if (!retObject->IsEmpty()) {
            Variant value = retObject->GetValue();

            // Ensure that the value type matches the return type.
            if (!_CheckReturnType(value)) {
                assert(false && "Method return type does not match assignment!");
            }

            retValue = retObject->GetValue();
        }
    }

    // Clean up our clone.
    SymplVMInstance->RemoveObject(clone->GetPath());

    return retValue;
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
        auto argObj = GetScope()->FindChildByName(argIt->GetName().c_str());
        assert(!argObj->IsEmpty() && "Invalid argument given for method");

        argObj->SetValue(args[argIndex]);

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
            argIt->SetValue(stat.Ptr()->Evaluate());
        }
    }
}

void ScriptMethod::_ProcessCallStatements()
{
    for (auto entryIt : _CallStatements) {
        entryIt->Statement->Build(entryIt->Variable.Ptr());
        entryIt->Variable->SetValue(entryIt->Statement->Evaluate());
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

    // Add the arguments.
    for (auto entryIt : _Args) {
        auto argObj = to_method(clone)->GetScope()->FindChildByName(entryIt->GetName().c_str());
        if (!argObj->IsEmpty()) {
            to_method(clone)->AddArg(argObj);
        }
    }

    // Add the statements.
    for (auto entryIt : _CallStatements) {
        auto callObj = to_method(clone)->GetScope()->FindChildByName(entryIt->Variable->GetName().c_str());
        if (!IsNullObject(callObj) && !callObj->IsEmpty()) {
            to_method(clone)->AddStatement(callObj, entryIt->Statement->Clone(to_method(clone)->GetScope()));
        }
    }

    return clone;
}

ScriptObject* ScriptMethod::GetScope()
{
    if (!_Scope.IsValid()) {
        _Scope = FindChildByName(".");
    }
    return _Scope.Ptr();
}
