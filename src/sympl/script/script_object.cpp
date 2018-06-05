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
#include <sympl/script/script_vm.h>
#include <sympl/core/string_buffer.h>
#include <sympl/util/string_helper.h>

#include <fmt/format.h>
sympl_namespaces

ScriptObject ScriptObject::Empty;

ScriptObject::ScriptObject()
{
    __Construct();
}

void ScriptObject::__Construct()
{
    _Name = "";
    _Path = "";
    _Parent = nullptr;
    _Context = nullptr;
    _Type = ScriptObjectType::Empty;
}

void ScriptObject::_Initialize(const char* name, const char* path, ScriptObject* parent)
{
    _Name = name;
    _CleanName = name;
    _Path = path;
    _Parent = parent;

    if (_Parent.IsValid()) {
        CreateContext(parent->GetContext());
    } else {
        CreateContext(ScriptVMInstance->GetContext());
    }
}

void ScriptObject::AddChild(ScriptObject* scriptObject)
{
    scriptObject->_SetNestLevel(_NestLevel + 1);
    _Children.emplace_back(scriptObject);
}

Variant ScriptObject::Evaluate(const Urho3D::PODVector<Variant>& args)
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
        name = fmt::format("{0}:{1}", _Name, StringHelper::GenerateRandomStr(5));
    }

    auto clone = _OnCloneCreateObject(name, parent);
    clone->SetCleanName(_Name);

    clone->SetValue(_Value);

    // Clone the children!
    for (auto entryIt : _Children) {
        // Don't try to clone the clone!
        if (entryIt == clone) {
            continue;
        }

        // Check if the clone already has the child.
        auto child = clone->FindChildByName(entryIt->GetName().c_str(), false);
        if (child->IsEmpty()) {
            entryIt->Clone(clone, false);
        }
    }

    return clone;
}

ScriptObject* ScriptObject::_OnCloneCreateObject(const std::string& name, ScriptObject* parent)
{
    auto clone = &ScriptObject::Empty;
    clone = ScriptVMInstance->CreateObject(name.c_str(), _Type, parent);
    return clone;
}

void ScriptObject::CreateContext(ScriptContext* context)
{
    auto newContext = alloc_ref(ScriptContext);
    newContext->SetScriptObject(this);
    newContext->SetParentContext(context);
    SetContext(newContext);
}

ScriptObject* ScriptObject::FindChildByName(const char* name, bool useCleanName)
{
    for (auto entryIt : _Children) {
        if (useCleanName) {
            if (strcmp(entryIt->GetCleanName().c_str(), name) == 0) {
                return entryIt.Ptr();
            }
        } else {
            if (strcmp(entryIt->GetName().c_str(), name) == 0) {
                return entryIt.Ptr();
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
    return ScriptVMInstance->FindObjectByPath(fmt::format("{0}", name));
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

    auto childIt = std::begin(_Children);
    while (childIt != std::end(_Children)) {
        if (childIt->Ptr() == child) {
            _Children.erase(childIt);
            break;
        }
        ++childIt;
    }
}

bool ScriptObject::Release()
{
    // Attempt to release the object reference.
    if (!Object::Release()) {
        return false;
    }

    // Update the parent context (if we have one).
    if (!_Context->GetParentContext()->IsEmpty()) {
        _Context->GetParentContext()->RemoveVariable(this);
    }

    // Remove from the context.
    _Context->Release();

    // Remove the children.
    for (auto childIt : _Children) {
        childIt->Release();
    }
    _Children.clear();

    return true;
}

WeakPtr<ScriptObject> ScriptObject::GetParent() const
{
    return _Parent;
}

void ScriptObject::_SetPath(const char* path)
{
    _Path = path;
}

void ScriptObject::_SetType(ScriptObjectType type)
{
    _Type = type;
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
    return "empty";
}

std::string ScriptObject::Print()
{
    auto buffer = alloc_ref(StringBuffer);
    buffer->Append(fmt::format("{0} ({1}) -> {2}", _Name, _Path, GetTypeAsString()).c_str());
    buffer->Append("\n");

    for (auto childIt : _Children) {
        _PrintChild(childIt.Ptr(), buffer);
    }

    std::string results = buffer->CStr();
    free_ref(buffer);

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
        _PrintChild(childIt.Ptr(), buffer);
    }
}
