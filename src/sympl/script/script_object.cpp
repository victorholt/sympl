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
#include "script_object.h"
#include "../core/string_buffer.h"

#include <fmt/format.h>
sympl_namespaces

ScriptObject ScriptObject::Empty;

ScriptObject::ScriptObject()
{
    _Name = "";
    _Path = "";
    _Parent = nullptr;
    _Type = ScriptObjectType::Empty;
}

ScriptObject::~ScriptObject()
{

}

void ScriptObject::_Initialize(const char* name, const char* path, ScriptObject* parent)
{
    _Name = name;
    _Path = path;
    _Parent = parent;

    if (!IsNullObject(parent)) {
        parent->AddRef();
    }
}

void ScriptObject::_AddChild(ScriptObject* scriptObject)
{
    scriptObject->AddRef();
    scriptObject->_SetNestLevel(_NestLevel + 1);
    _Children.push_back(scriptObject);
}

bool ScriptObject::Execute(Variant*& result)
{
    return false;
}

ScriptObject& ScriptObject::GetParent() const
{
    if (!IsNullObject(_Parent)) {
        _Parent->AddRef();
        return *_Parent;
    }
    return ScriptObject::Empty;
}

const Variant& ScriptObject::GetValue() const
{
    return _Value;
}

const std::vector<ScriptObject*>& ScriptObject::GetChildren() const
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

bool ScriptObject::Free()
{
    if (Ref::Free()) {
        return false;
    }
}

std::string ScriptObject::Print()
{
    auto buffer = alloc_ref(StringBuffer);
    buffer->Append(fmt::format("{0} ({1})", _Name, _Path).c_str());
    buffer->Append("\n");

    for (auto childIt : _Children) {
        _PrintChild(childIt, buffer);
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
        _PrintChild(childIt, buffer);
    }
}