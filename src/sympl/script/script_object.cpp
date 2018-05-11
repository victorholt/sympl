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
#include <sympl/script/script_statement.h>
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
}

ScriptObject::~ScriptObject()
{
    _Children.clear();
}

void ScriptObject::_Initialize(const char* name, const char* path, ScriptObject* parent)
{
    _Name = name;
    _Path = path;
    _Parent = parent;
}

void ScriptObject::_AddChild(ScriptObject* scriptObject)
{
    scriptObject->_SetNestLevel(_NestLevel + 1);
    _Children[scriptObject->GetPath()] = scriptObject;
}

bool ScriptObject::Evaluate(Variant *&result)
{
    return false;
}

const SharedRef<ScriptObject>& ScriptObject::GetParent() const
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
    _Value = value;
}

ScriptStatement* ScriptObject::GetValue()
{
    return _Value.Ptr();
}

std::string ScriptObject::GetTypeString() const
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
    buffer->Append(fmt::format("{0} ({1}) -> {2}", _Name, _Path, GetTypeString()).c_str());
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
