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
    int argIndex = 0;
    for (auto argIt : _Args) {
        // Ensure we have args.
        if (argIndex >= args.size()) {
            break;
        }

        Variant argValue = args[argIndex];
        auto argObj = to_method(clone)->GetScope()->FindChildByName(argIt->GetName().c_str());
        assert(!argObj->IsEmpty() && "Invalid argument given for method");

        argObj->SetValue(args[argIndex]);

        argIndex++;
    }

    // Go through all of the call statements.
    to_method(clone)->_ProcessCallStatements();

    // Check if we have a return variable.
    auto retObject = to_method(clone)->GetScope()->FindChildByName("return");
    if (!retObject->IsEmpty()) {
        Variant value = retObject->GetValue();
        retValue = retObject->GetValue();
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

void ScriptMethod::_ProcessCallStatements()
{
    for (auto entryIt : _CallStatements) {
        entryIt->Variable->SetValue(entryIt->Statement->Evaluate());
    }
}

void ScriptMethod::AddArg(ScriptObject* arg)
{
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

    // Add the arguments.
    for (auto entryIt : _Args) {
        auto argObject = clone->FindChildByName(entryIt->GetName().c_str());
        if (!IsNullObject(argObject) && !argObject->IsEmpty()) {
            to_method(clone)->AddArg(argObject);
        }
    }

    // Add the statements.
    for (auto entryIt : _CallStatements) {
        auto callObject = to_method(clone)->GetScope()->FindChildByName(entryIt->Variable->GetName().c_str());
        if (!IsNullObject(callObject) && !callObject->IsEmpty()) {
            to_method(clone)->AddStatement(callObject, entryIt->Statement->Clone(to_method(clone)->GetScope()));
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