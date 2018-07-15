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
    _ArgString = mem_alloc_ref(StringBuffer);
}

void ScriptMethod::_Initialize(const char* name, const char* path, ScriptObject* parent)
{
    ScriptObject::_Initialize(name, path, parent);

    // Create the scope.
    _Scope = ScriptVMInstance->CreateObject(SYMPL_SCOPE_NAME, ScriptObjectType::Object, this);
    _Scope->GetContext()->SetParentContext(_Context.Ptr());
}

Variant ScriptMethod::Evaluate(ScriptMethodArgs args)
{
    _Exit = false;

    _CopyArgs(args);
    _ProcessArgStatements();
    _ProcessCallStatements();

    // Delete anything created in the scope.
//    for (auto& child : GetScope()->GetChildren()) {
//        if (child->GetType() == ScriptObjectType::Method) {
//            continue;
//        }
//        if (child->RefCount() == 1) {
//            for (auto childEntry : child->GetChildren()) {
//                ScriptVMInstance->QueueDelete(childEntry.Ptr());
//            }
//        }
//    }

    return _Value;
}

Variant ScriptMethod::Evaluate()
{
    Urho3D::PODVector<Variant> args;
    return Evaluate(args);
}

void ScriptMethod::_CopyArgs(ScriptMethodArgs args)
{
    int argIndex = 0;

    unsigned asize = args.Size();
    int size = _Args.Size();
    for (int i = 0; i < size; i++) {
        ScriptObject* argIt = nullptr;
        if (_Args[i].GetType() != VariantType::Object) {
            continue;
        }
        argIt = static_cast<ScriptObject*>(_Args[i].GetObject());
        // Ensure we have args.
        if (argIndex >= size) {
            break;
        }

        Variant argValue;
        if (asize <= argIndex) {
            argValue = _Args[argIndex];
        } else {
            argValue = args[argIndex];
        }

        auto argObj = GetScope()->TraverseUpFindChildByName(argIt->GetName().c_str(), false);
        sympl_assert(!argObj->IsEmpty() && "Invalid argument given for method");

        argObj->SetValue(argValue);

        argIndex++;
    }
}

void ScriptMethod::_ProcessArgStatements()
{
    // String arguments will need to have statements
    // to execute their value.
    auto size = _Args.Size();
    for (unsigned i = 0; i < size; i++) {
        ScriptObject* argIt = nullptr;
        if (_Args[i].GetType() != VariantType::Object) {
            continue;
        }
        argIt = static_cast<ScriptObject*>(_Args[i].GetObject());
        Variant value = argIt->GetValue();

        if (value.GetType() == VariantType::StringBuffer) {
            auto resolver = SymplRegistry.Get<StatementResolver>();

            Variant argValue = resolver->Resolve(argIt->GetValue().GetStringBuffer()->CStr(), argIt);

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
    auto size = _CallStatements.size();
    for (unsigned i = 0; i < size; i++) {
        auto entryIt = _CallStatements[i];
        if (_Exit) return;

        auto varObject = entryIt->Variable;
        if (varObject.IsValid()) {
            varObject->GetContext()->SetCallerContext(GetScope()->GetContext());
        } else if (entryIt->VirtualObjectStr.IsValid()) {
            varObject = ScriptVMInstance->FindObjectByPath(
                    entryIt->VirtualObjectStr->CStr(),
                    GetScope()
            );
        }

        auto val = entryIt->Resolver->Resolve(entryIt->StatementStr->CStr(), varObject.Ptr());

        if (!val.IsEmpty()) {
            _Value = val;
            varObject->SetValue(_Value);
        }

        // Check if we're attempting to return out of the method.
        if (varObject->GetName() == "return") {
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
        if (argIt == arg) {
            return;
        }
    }

    arg->SetMeta("is_method_arg", true);
    _Args.Push(arg);

    // Add arg as a variable in the context.
//    GetScope()->GetContext()->AddVar(arg);
}

void ScriptMethod::AddStatement(ScriptObject* variable, const char* stmtStr)
{
    auto callStatement = alloc_bytes(MethodCallStatement);
    callStatement->Resolver = mem_alloc_ref(StatementResolver);
    callStatement->Variable = variable;
    callStatement->StatementStr = mem_alloc_ref(StringBuffer);
    callStatement->StatementStr->Append(stmtStr);

    _CallStatements.push_back(callStatement);
}

void ScriptMethod::AddVirtualStatement(const char* virtualObjectStr, const char* stmtStr)
{
    auto callStatement = alloc_bytes(MethodCallStatement);
    callStatement->Resolver = mem_alloc_ref(StatementResolver);
    callStatement->VirtualObjectStr = mem_alloc_ref(StringBuffer);
    callStatement->VirtualObjectStr->Append(virtualObjectStr);
    callStatement->StatementStr = mem_alloc_ref(StringBuffer);
    callStatement->StatementStr->Append(stmtStr);

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
        auto child = to_method(clone)->GetScope()->FindChildByName(entryIt->GetName().c_str(), false);
        if (child->IsEmpty()) {
            entryIt->Clone(to_method(clone)->GetScope(), false);
        }
    }

    // Add the arguments.
    for (auto entryIt : _Args) {
        ScriptObject* obj = nullptr;
        if (entryIt.GetType() != VariantType::Object) {
            continue;
        }
        obj = static_cast<ScriptObject*>(entryIt.GetObject());

        auto argObj = to_method(clone)->GetScope()->FindChildByName(obj->GetName().c_str(), false);
        if (!argObj->IsEmpty()) {
            to_method(clone)->AddArg(argObj);
        }
    }

    // Add the statements.
    for (auto entryIt : _CallStatements) {
        if (entryIt->Variable.IsValid()) {
            auto callObj = to_method(clone)->GetScope()->TraverseUpFindChildByName(entryIt->Variable->GetName().c_str(),
                                                                                   false);
            if (!IsNullObject(callObj) && !callObj->IsEmpty()) {
                to_method(clone)->AddStatement(callObj, entryIt->StatementStr->CStr());
            }
        } else if (entryIt->VirtualObjectStr.IsValid()) {
            to_method(clone)->AddVirtualStatement(entryIt->VirtualObjectStr->CStr(), entryIt->StatementStr->CStr());
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

void ScriptMethod::AddArgCache(const char* argString, Variant& argValue, unsigned argStringLength)
{
    auto cache = FindOrCreateArgCache(argString);
    if (IsNullObject(cache)) {
        return;
    }

    if (!cache->Args.empty()) {
        for (auto &arg : cache->Args) {
            if (arg.ArgValue == argValue) {
                return;
            }
        }
    }

    MethodArgCacheValue cacheValue;
    cacheValue.ArgValue = argValue;
    cacheValue.ArgStringLength = argStringLength;

    cache->Args.push_back(cacheValue);
}

MethodArgCache* ScriptMethod::FindOrCreateArgCache(const char* argString)
{
    // TODO: Think about not having a limit (or larger limit)...?
    if (IsImmediate() || strlen(argString) > 255) {
        return nullptr;
    }

    for (auto entry : _ArgCache) {
        if (strcmp(argString, entry.first.c_str()) == 0) {
            return entry.second;
        }
    }

    MethodArgCache* cache = nullptr;
    cache = mem_alloc_object(MethodArgCache);
    memcpy(cache->ArgString, argString, strlen(argString));
    _ArgCache[argString] = cache;

    return cache;
}

bool ScriptMethod::HasArgCache(const char* argString)
{
    for (auto& entry : _ArgCache) {
        if (strcmp(argString, entry.first.c_str()) == 0) {
            return true;
        }
    }
    return false;
}

void ScriptMethod::ClearArgCaches()
{
    for (auto& entry : _ArgCache) {
        delete entry.second;
    }
    _ArgCache.clear();
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
    _CallStatements.clear();
    _Args.Clear();

    ClearArgCaches();

    return true;
}
