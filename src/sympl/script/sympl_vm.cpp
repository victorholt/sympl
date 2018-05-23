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
#include <sympl/core/string_buffer.h>
#include <sympl/script/sympl_vm.h>
#include <sympl/script/interpreter.h>
#include <sympl/script/methods/method_registry.h>
#include <sympl/io/script_reader.h>

#include <fmt/format.h>
sympl_namespaces

SymplVM* SymplVM::_Instance = nullptr;

SymplVM::SymplVM()
{

}

SymplVM::~SymplVM()
{
    if (!IsNullObject(_Instance)) {
        _Instance->Shutdown();
    }
}

void SymplVM::Startup()
{
    _MethodRegistry = alloc_ref(MethodRegistry);
    _MethodRegistry->_Initialize();
}

void SymplVM::Shutdown()
{
    for (auto entryIt : _ObjectMap) {
        auto scriptObject = entryIt.second;
        if (scriptObject->GetParent().IsValid()) {
            auto parent = scriptObject->GetParent().Ptr();
            parent->RemoveChild(scriptObject->GetName().c_str());
        }
    }
    _ObjectMap.clear();
}

void SymplVM::GC()
{
    for (auto entryIt : _ObjectMap) {
        if (!entryIt.second.IsValid() || entryIt.second.RefCount() == 1) {
            _ObjectMap.erase(entryIt.first);
        }
    }
}

Interpreter* SymplVM::LoadFile(const char* filePath)
{
    SharedRef<ScriptReader> reader = alloc_ref(ScriptReader);
    reader->ReadFile(filePath);

    auto interpret = alloc_ref(Interpreter);
    interpret->_SetReader(reader.Ptr());
    interpret->_Parse();

    return interpret;
}

Interpreter* SymplVM::LoadString(const char* str)
{
    SharedRef<ScriptReader> reader = alloc_ref(ScriptReader);
    reader->ReadString(str);

    auto interpret = alloc_ref(Interpreter);
    interpret->_SetReader(reader.Ptr());
    interpret->_Parse();

    return interpret;
}

ScriptObject* SymplVM::CreateObject(const char* name, ScriptObjectType type, ScriptObject* parent)
{
    std::string path = _BuildPath(name, parent);
    auto* searchObj = FindObject(path);
    if (!searchObj->IsEmpty()) {
        return searchObj;
    }

    SharedRef<ScriptObject> scriptObject;

    if (type == ScriptObjectType::Method) {
        scriptObject = alloc_ref(ScriptMethod);
    } else {
        scriptObject = alloc_ref(ScriptObject);
    }
    scriptObject->_Initialize(name, path.c_str(), parent);
    scriptObject->_SetType(type);
    _ObjectMap[scriptObject->GetPath()] = scriptObject;

    if (!IsNullObject(parent)) {
        parent->AddChild(scriptObject.Ptr());
    }

    return scriptObject.Ptr();
}

ScriptObject* SymplVM::CreateObject(const char* name, ScriptObject* parent)
{
    return CreateObject(name, ScriptObjectType::Object, parent);
}

bool SymplVM::AddObject(ScriptObject* scriptObject, ScriptObject* parent)
{
    // Generate a path for the object.
    // Ensure the object does not already
    // exist in the VM!
    std::string path = _BuildPath(scriptObject->GetName().c_str(), parent);
    auto* searchObj = FindObject(path);
    if (!searchObj->IsEmpty()) {
        return (searchObj == scriptObject);
    }

    scriptObject->_Initialize(scriptObject->GetName().c_str(), path.c_str(), parent);
    _ObjectMap[scriptObject->GetPath()] = scriptObject;

    if (!IsNullObject(parent)) {
        parent->AddChild(scriptObject);
    }

    return true;
}

ScriptObject* SymplVM::FindObject(const std::string& path)
{
    auto entryIt = _ObjectMap.find(path);
    if (entryIt == _ObjectMap.end()) {
        return &ScriptObject::Empty;
    }

    return entryIt->second.Ptr();
}

ScriptObject* SymplVM::FindObject(ScriptObject* scopeObject, const std::string& objectName)
{
    if (!IsNullObject(scopeObject) && !scopeObject->IsEmpty()) {
        return FindObject(fmt::format("{0}.{1}", scopeObject->GetPath(), objectName));
    }

    return FindObject(fmt::format(".{0}", objectName));
}

std::string SymplVM::_BuildPath(const char* name, ScriptObject* parent)
{
    if (!IsNullObject(parent)) {
        return fmt::format("{0}.{1}", parent->GetPath(), name);
    }
    return fmt::format(".{0}", name);
}

void SymplVM::RemoveObject(const std::string& path)
{
    auto scriptObject = FindObject(path);

    if (scriptObject->GetParent().IsValid()) {
        auto parent = scriptObject->GetParent().Ptr();
        parent->RemoveChild(scriptObject->GetName().c_str());
    }
    _ObjectMap.erase(scriptObject->GetPath());
}

MethodRegistry* SymplVM::GetMethodRegistry()
{
    return _MethodRegistry.Ptr();
}

std::string SymplVM::PrintObjects()
{
    auto buffer = alloc_ref(StringBuffer);
    for (auto entryIt : _ObjectMap) {
        // Don't print out objects with parents.
        if (entryIt.second->GetParent().IsValid()) {
            continue;
        }
        buffer->Append(entryIt.second->Print().c_str());
    }

    std::string results = buffer->CStr();
    free_ref(StringBuffer, buffer);

    return results;
}
