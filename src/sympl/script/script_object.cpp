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
#include <sympl/script/script_object.h>
#include <sympl/script/script_method.h>
#include <sympl/script/script_statement.h>
#include <sympl/script/sympl_vm.h>
#include <sympl/core/string_buffer.h>

#include <fmt/format.h>
sympl_namespaces

ScriptObject ScriptObject::Empty;

ScriptObject::ScriptObject()
{
    _Name = "";
    _Path = "";
    _Parent = nullptr;
    _Type = ScriptObjectType::Empty;
    _Context = &ScriptContext::Empty;
}

ScriptObject::~ScriptObject()
{
}

void ScriptObject::_Initialize(const char* name, const char* path, ScriptObject* parent)
{
    _Name = name;
    _CleanName = name;
    _Path = path;
    _Parent = parent;
}

void ScriptObject::AddChild(ScriptObject* scriptObject)
{
    scriptObject->_SetNestLevel(_NestLevel + 1);
    _Children[scriptObject->Guid()] = scriptObject;
}

Variant ScriptObject::Evaluate(const std::vector<Variant>& args)
{
    return Variant::Empty;
}

Variant ScriptObject::Evaluate()
{
    return Variant::Empty;
}

ScriptObject* ScriptObject::Clone()
{
    return Clone(_Parent.Ptr(), true);
}

ScriptObject* ScriptObject::Clone(ScriptObject* parent, bool uniqueName)
{
    // Generate a random name for the object.
    std::string name = _Name;
    if (uniqueName) {
        auto guid = xg::newGuid();
        std::stringstream guidName;
        guidName << guid;
        name = fmt::format("{0}:{1}", _Name, guidName.str());
    }

    ScriptObject* clone = _OnCloneCreateObject(name, parent);
    clone->SetCleanName(_Name);

    clone->SetValue(_Value);

    // Clone the children!
    for (auto entryIt : _Children) {
        // Don't try to clone the clone!
        if (entryIt.second == clone) {
            continue;
        }

        // Check if the clone already has the child.
        auto child = clone->FindChildByName(entryIt.second->GetName().c_str(), false);
        if (child->IsEmpty()) {
            entryIt.second->Clone(clone, false);
        }
    }

    return clone;
}

ScriptObject* ScriptObject::_OnCloneCreateObject(const std::string& name, ScriptObject* parent)
{
    ScriptObject* clone = &ScriptObject::Empty;
    clone = SymplVMInstance->CreateObject(name.c_str(), _Type, parent);
    return clone;
}

ScriptObject* ScriptObject::FindChildByName(const char* name, bool useCleanName)
{
    for (auto entryIt : _Children) {
        if (useCleanName) {
            if (strcmp(entryIt.second->GetCleanName().c_str(), name) == 0) {
                return entryIt.second.Ptr();
            }
        } else {
            if (strcmp(entryIt.second->GetName().c_str(), name) == 0) {
                return entryIt.second.Ptr();
            }
        }
    }
    return &ScriptObject::Empty;
}

ScriptObject* ScriptObject::TraverseUpFindChildByName(const char* name, bool useCleanName)
{
    auto scriptObject = FindChildByName(name, useCleanName);
    if (!scriptObject->IsEmpty()) {
        return scriptObject;
    }

    // Attempt to find from the parent.
    if (_Parent.IsValid()) {
        scriptObject = _Parent->TraverseUpFindChildByName(name);
        if (!scriptObject->IsEmpty()) {
            return scriptObject;
        }
    }

    // Find in the main scope.
    return SymplVMInstance->FindObjectByPath(fmt::format("{0}", name));
}

ScriptObject* ScriptObject::FindCalledByMethod()
{
    // Traverse up to find a non-immediate method.
    /// Find the parent method and signal an exit.
    auto parent = GetParent();

    while (parent.IsValid()) {
        if (parent->GetType() == ScriptObjectType::Method && !to_method(parent.Ptr())->IsImmediate()) {
            return parent.Ptr();
        }
        parent = parent->GetParent();
    }

    return &ScriptObject::Empty;
}

void ScriptObject::RemoveChild(const char* name)
{
    auto child = FindChildByName(name);
    if (child->IsEmpty()) {
        return;
    }
    _Children.erase(child->GetPath());
}

void ScriptObject::Release()
{
    for (auto childIt : _Children) {
        childIt.second->Release();
    }
    _Children.clear();
}

SharedRef<ScriptObject> ScriptObject::GetParent() const
{
    return _Parent;
}

const std::unordered_map<std::string, SharedRef<ScriptObject>>& ScriptObject::GetChildren() const
{
    return _Children;
}

void ScriptObject::_SetPath(const char* path)
{
    _Path = path;
}

void ScriptObject::_SetType(ScriptObjectType type)
{
    _Type = type;
}

void ScriptObject::SetValue(ScriptStatement* value)
{
    _Value = value->Evaluate();
}

std::string ScriptObject::GetTypeAsString() const
{
    if (_Type == ScriptObjectType::Object) {
        return "object";
    }
    if (_Type == ScriptObjectType::Variable) {
        return "var";
    }
    if (_Type == ScriptObjectType::Array) {
        return "array";
    }
    if (_Type == ScriptObjectType::Method) {
        return "method";
    }
    if (_Type == ScriptObjectType::Statement) {
        return "statement";
    }
    if (_Type == ScriptObjectType::Empty) {
        return "empty";
    }
}

std::string ScriptObject::Print()
{
    auto buffer = alloc_ref(StringBuffer);
    buffer->Append(fmt::format("{0} ({1}) -> {2}", _Name, _Path, GetTypeAsString()).c_str());
    buffer->Append("\n");

    for (auto childIt : _Children) {
        _PrintChild(childIt.second.Ptr(), buffer);
    }

    std::string results = buffer->CStr();
    free_ref(StringBuffer, buffer);

    return results;
}

void ScriptObject::_PrintChild(ScriptObject* childObj, StringBuffer*& buffer)
{
    for (unsigned i = 0; i < childObj->GetNestLevel(); i++) {
        buffer->Append("--> ");
    }
    buffer->Append(fmt::format("{0} ({1})", childObj->GetName(), childObj->GetPath()).c_str());
    buffer->Append("\n");

    for (auto childIt : childObj->GetChildren()) {
        _PrintChild(childIt.second.Ptr(), buffer);
    }
}
