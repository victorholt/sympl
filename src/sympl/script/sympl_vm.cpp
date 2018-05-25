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
    Release();
}

void SymplVM::Startup()
{
    _MethodRegistry = alloc_ref(MethodRegistry);
    _MethodRegistry->_Initialize();
}

void SymplVM::Shutdown()
{
    _MethodRegistry.Release();

    for (auto entryIt : _ObjectMap) {
        entryIt.second->Release();
    }
    _ObjectMap.clear();
}

void SymplVM::GC()
{
    std::vector<uint64_t> removeGuids;

    for (auto entryIt : _ObjectMap) {
        if (!entryIt.second.IsValid() || entryIt.second.RefCount() == 1) {
            entryIt.second->Release();
            removeGuids.push_back(entryIt.first);
        }
    }

    for (auto guid : removeGuids) {
        _ObjectMap.erase(guid);
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
    auto* searchObj = FindObjectByPath(path);

    assert(searchObj->IsEmpty() && "Attempted to create duplicate object in the VM!");

    ScriptObject* scriptObject = nullptr;

    if (type == ScriptObjectType::Method) {
        scriptObject = alloc_ref(ScriptMethod);
    } else {
        scriptObject = alloc_ref(ScriptObject);
    }
    scriptObject->_Initialize(name, path.c_str(), parent);
    scriptObject->_SetType(type);

    if (!IsNullObject(parent) && !parent->IsEmpty()) {
        parent->AddChild(scriptObject);
    } else {
        _ObjectMap[scriptObject->MemAddress()] = scriptObject;
    }

    return scriptObject;
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
    auto searchObj = FindObjectByPath(path);
    if (!searchObj->IsEmpty()) {
        assert (false && "Attempted to create duplicate object in the VM");
    }

    scriptObject->_Initialize(scriptObject->GetName().c_str(), path.c_str(), parent);

    if (!IsNullObject(parent) && !parent->IsEmpty()) {
        parent->AddChild(scriptObject);
    } else {
        _ObjectMap[scriptObject->MemAddress()] = scriptObject;
    }

    return true;
}

ScriptObject* SymplVM::FindObjectByPath(const std::string& path)
{
    std::string delimiter = ".";

    size_t pos = 0;
    std::string parseStr = path;
    std::string token;
    std::string currentPath;

    auto currentObject = &ScriptObject::Empty;
    auto resultObject = &ScriptObject::Empty;

    if ((pos = parseStr.find(delimiter)) == std::string::npos) {
        for (auto entryIt : _ObjectMap) {
            if (entryIt.second->GetPath() == path) {
                resultObject = entryIt.second.Ptr();
                break;
            }
        }
    } else {
        while ((pos = parseStr.find(delimiter)) != std::string::npos) {
            token = parseStr.substr(0, pos);
            parseStr.erase(0, pos + delimiter.length());

            // Search through the object map and the children.
            if (currentObject->IsEmpty()) {
                currentPath.append(token);
                for (auto entryIt : _ObjectMap) {
                    if (entryIt.second->GetPath() == currentPath) {
                        currentObject = entryIt.second.Ptr();
                        break;
                    }
                }
                if (currentObject->IsEmpty()) break;
            } else {
                // Search the children.
                currentPath.append(".");
                currentPath.append(token);

                for (auto entryIt : currentObject->GetChildren()) {
                    if (entryIt.second->GetPath() == currentPath) {
                        currentObject = entryIt.second.Ptr();
                        break;
                    }
                }
                if (currentObject->IsEmpty()) break;
            }
        }

        // Handle the last part of the path.
        if (!currentObject->IsEmpty()) {
            currentPath.append(parseStr);
            for (auto entryIt : currentObject->GetChildren()) {
                if (entryIt.second->GetPath() == currentPath) {
                    resultObject = entryIt.second.Ptr();
                    break;
                }
            }
        }
    }
    return resultObject;
}

ScriptObject* SymplVM::FindObjectByScope(ScriptObject* scopeObject, const std::string& objectName)
{
    if (!IsNullObject(scopeObject) && !scopeObject->IsEmpty()) {
        return FindObjectByPath(fmt::format("{0}.{1}", scopeObject->GetPath(), objectName));
    }

    return FindObjectByPath(fmt::format("{0}", objectName));
}

std::string SymplVM::_BuildPath(const char* name, ScriptObject* parent)
{
    if (!IsNullObject(parent)) {
        return fmt::format("{0}.{1}", parent->GetPath(), name);
    }
    return fmt::format("{0}", name);
}

void SymplVM::RemoveObject(ScriptObject* scriptObject)
{
    if (IsNullObject(scriptObject) || scriptObject->IsEmpty()) {
        return;
    }

    if (scriptObject->GetParent().IsValid()) {
        auto parent = scriptObject->GetParent().Ptr();
        parent->RemoveChild(scriptObject->GetName().c_str());
    }
    scriptObject->Release();
    _ObjectMap.erase(scriptObject->MemAddress());
}

void SymplVM::RemoveObject(const std::string& path)
{
    auto scriptObject = FindObjectByPath(path);

    if (scriptObject->IsEmpty()) {
        return;
    }

    if (scriptObject->GetParent().IsValid()) {
        auto parent = scriptObject->GetParent().Ptr();
        parent->RemoveChild(scriptObject->GetName().c_str());
    }
    scriptObject->Release();
    _ObjectMap.erase(scriptObject->MemAddress());
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
