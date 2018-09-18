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
#include <sympl/script/script_context.h>
#include <sympl/script/script_object.h>
#include <sympl/script/statement_cache.h>

#include <fmt/format.h>
sympl_namespaces

ScriptContext ScriptContext::Empty;

ScriptContext::ScriptContext()
{
    _StatementCache = mem_alloc_ref(StatementCache);
}

void ScriptContext::__Construct()
{
}

//void ScriptContext::AddVar(ScriptObject* varObject)
//{
//    // Check to ensure we don't already have this variable.
//    auto varIt = _VarList.Find(varObject);
//    if (varIt != _VarList.End()) {
//        return;
//    }
//    _VarList.Push(varObject);
//}
//
//void ScriptContext::AddObject(ScriptObject* obj)
//{
//    // Check to ensure we don't already have this variable.
//    auto objIt = _ObjectList.Find(obj);
//    if (objIt != _ObjectList.End()) {
//        return;
//    }
//    _ObjectList.Push(obj);
//}

ScriptObject* ScriptContext::FindVariable(const char* name, bool traverse)
{
//    auto varIt = _VarList.Begin();
//    while (varIt != _VarList.End()) {
//        if (strcmp((*varIt)->GetName().c_str(), name) == 0) {
//            return *varIt;
//        }
//        varIt++;
//    }
//    if (!traverse) return &ScriptObject::Empty;

    // Check the children in this context's object.
    auto children = _Object->GetChildren();
    for (auto childIt : children) {
        if (strcmp(childIt->GetName().c_str(), name) == 0) {
//            AddVar(childIt.Ptr());
            return childIt.Ptr();
        }
    }

    // Search the context of the caller if we have one (methods).
    if (_CallerContext.IsValid()) {
        auto var = _CallerContext->FindVariable(name);
        if (!var->IsEmpty()) {
//            AddVar(var);
            return var;
        }
    }

    // Check if we have a parent scope and go up the chain.
    if (_ParentContext.IsValid()) {
        auto var = _ParentContext->FindVariable(name);
        if (!var->IsEmpty()) {
//            AddVar(var);
            return var;
        }
    }

    return &ScriptObject::Empty;
}

ScriptObject* ScriptContext::FindObject(const char* name, bool traverse)
{
    // Search the context of the caller if we have one (methods).
    if (_CallerContext.IsValid()) {
        auto obj = _CallerContext->FindObject(name);
        if (!obj->IsEmpty()) {
//            AddObject(obj);
            return obj;
        }
    }

    // Check if we have a parent scope and go up the chain.
    if (_ParentContext.IsValid()) {
        auto obj = _ParentContext->FindObject(name);
        if (!obj->IsEmpty()) {
//            AddObject(obj);
            return obj;
        }
    }

    return &ScriptObject::Empty;
}

void ScriptContext::RemoveVariable(ScriptObject* scriptObject)
{
    if (scriptObject->GetType() != ScriptObjectType::Variable) {
        return;
    }
//    _VarList.Remove(scriptObject);
}

//void ScriptContext::CopyVarsTo(ScriptContext* context)
//{
//    for (auto var : _VarList) {
//        auto findVar = FindVariable(var->GetName().c_str(), false);
//        if (!findVar->IsEmpty()) {
//            context->AddVar(var);
//        }
//    }
//}

void ScriptContext::SetScriptObject(ScriptObject* scriptObject)
{
    if (!scriptObject->IsEmpty()) {
        _Object = scriptObject;
    }
}

ScriptObject* ScriptContext::GetScriptObject()
{
    return _Object.IsValid() ? _Object.Ptr() : &ScriptObject::Empty;
}

ScriptContext* ScriptContext::GetParentContext() const { return _ParentContext.IsValid() ? _ParentContext.Ptr() : &ScriptContext::Empty; }

ScriptContext* ScriptContext::GetCallerContext() const { return _CallerContext.IsValid() ? _CallerContext.Ptr() : &ScriptContext::Empty; }

bool ScriptContext::IsEmpty() const { return !_Object.IsValid(); }

StatementCache* ScriptContext::GetStatementCache() const {
    if (!_StatementCache) {
        return nullptr;
    }
    return _StatementCache;
}

bool ScriptContext::Release()
{
    mem_free_ref(StatementCache, _StatementCache);
//    _VarList.Clear();
//    _ObjectList.Clear();
    return true;
}

