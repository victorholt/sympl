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
    _Type = ScriptObjectType::Empty;
}

void ScriptObject::Initialize(const std::string& name, const std::string& path, ScriptObjectType type, ScriptObject* parent)
{
    _Name = name;
    _CleanName = name;
    _Path = path;
    _Type = type;
    _Parent = parent;
}

void ScriptObject::AddChild(ScriptObject* ref)
{
    ref->SetParent(this);
    _Children.emplace_back(ref);
}

void ScriptObject::RemoveChild(ScriptObject* ref)
{
    auto childIt = std::begin(_Children);
    while (childIt != std::end(_Children)) {
        if (childIt->Ptr() == ref) {
            _Children.erase(childIt);
            break;
        }
        ++childIt;
    }
}

bool ScriptObject::IsChild(ScriptObject* ref)
{
    auto childIt = std::begin(_Children);
    while (childIt != std::end(_Children)) {
        if (childIt->Ptr() == ref) {
            return true;
        }
        ++childIt;
    }
    return false;
}

ScriptObject* ScriptObject::FindChildByName(const std::string& name, bool traverseUp)
{
    auto childIt = std::begin(_Children);
    while (childIt != std::end(_Children)) {
        if (childIt->Ptr()->GetName() == name) {
            return childIt->Ptr();
        }
        ++childIt;
    }

    // If we have not found the child, check if we want to traverse up the tree.
    if (traverseUp && _Parent.IsValid()) {
        auto child = _Parent->FindChildByName(name, traverseUp);
        if (!child->IsEmpty()) {
            return child;
        }
    }

    return &ScriptObject::Empty;
}

ScriptObject* ScriptObject::FindChildByPath(const std::string& path, bool traverseUp)
{
    auto childIt = std::begin(_Children);
    while (childIt != std::end(_Children)) {
        if (childIt->Ptr()->GetPath() == path) {
            return childIt->Ptr();
        }
        ++childIt;
    }

    // If we have not found the child, check if we want to traverse up the tree.
    if (traverseUp && _Parent.IsValid()) {
        auto child = _Parent->FindChildByPath(path, traverseUp);
        if (!child->IsEmpty()) {
            return child;
        }
    }

    return &ScriptObject::Empty;
}

ScriptObject* ScriptObject::FindChildByAddress(const std::string& address, bool traverseUp)
{
    auto childIt = std::begin(_Children);
    while (childIt != std::end(_Children)) {
        if (childIt->Ptr()->GetObjectAddress() == address) {
            return childIt->Ptr();
        }
        ++childIt;
    }

    // If we have not found the child, check if we want to traverse up the tree.
    if (traverseUp && _Parent.IsValid()) {
        auto child = _Parent->FindChildByAddress(address, traverseUp);
        if (!child->IsEmpty()) {
            return child;
        }
    }

    return &ScriptObject::Empty;
}

std::string ScriptObject::GetTypeAsString() const
{
    if (_Type == ScriptObjectType::Object) {
        return "object";
    }
    if (_Type == ScriptObjectType::Reference) {
        return "object (reference)";
    }
    if (_Type == ScriptObjectType::Array) {
        return "array";
    }
    if (_Type == ScriptObjectType::Method) {
        return "method";
    }
    return "empty";
}

bool ScriptObject::Release()
{
    return true;
}
