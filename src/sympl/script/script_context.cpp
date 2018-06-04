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

#include <fmt/format.h>
sympl_namespaces

ScriptContext ScriptContext::Empty;

ScriptContext::ScriptContext()
{
}

ScriptContext::~ScriptContext()
{
    Release();
}

void ScriptContext::__Construct()
{

}

void ScriptContext::AddVar(ScriptObject* varObject)
{
    // Check to ensure we don't already have this variable.
    auto varIt = std::begin(_VarList);
    while (varIt != std::end(_VarList)) {
        if ((*varIt)->GetName() == varObject->GetName()) {
            _VarList.erase(varIt);
            break;
        }
        varIt++;
    }

    _VarList.emplace_back(varObject);
}

ScriptObject* ScriptContext::FindVariable(const char* name, bool traverse)
{
    auto varIt = std::begin(_VarList);
    while (varIt != std::end(_VarList)) {
        if (strcmp((*varIt)->GetName().c_str(), name) == 0) {
            return *varIt;
        }
        varIt++;
    }
    if (!traverse) return &ScriptObject::Empty;

    // Search the context of the caller if we have one (methods).
    if (_CallerContext.IsValid()) {
        return _CallerContext->FindVariable(name);
    }

    // Check if we have a parent scope and go up the chain.
    if (_ParentContext.IsValid()) {
        auto var = _ParentContext->FindVariable(name);
        if (!var->IsEmpty()) {
            return var;
        }
    }

    return &ScriptObject::Empty;
}

void ScriptContext::RemoveVariable(ScriptObject* scriptObject)
{
    if (scriptObject->GetType() != ScriptObjectType::Variable) {
        return;
    }

    auto varIt = std::begin(_VarList);
    while (varIt != std::end(_VarList)) {
        if ((*varIt) == scriptObject) {
            _VarList.erase(varIt);
            return;
        }
        varIt++;
    }
}

void ScriptContext::CopyVarsTo(ScriptContext* context)
{
    for (auto var : _VarList) {
        auto findVar = FindVariable(var->GetName().c_str(), false);
        if (!findVar->IsEmpty()) {
            context->AddVar(var);
        }
    }
}

void ScriptContext::SetScriptObject(ScriptObject* scriptObject)
{
    _Object = scriptObject;
}

ScriptObject* ScriptContext::GetScriptObject()
{
    return _Object.IsValid() ? _Object.Ptr() : &ScriptObject::Empty;
}

bool ScriptContext::Release()
{
    _Object.Release();
    _VarList.clear();
    return true;
}

