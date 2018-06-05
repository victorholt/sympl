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
#include <sympl/script/script_vm.h>
#include <sympl/script/interpreter.h>
#include <sympl/script/method_registry.h>
#include <sympl/script/script_reader.h>

#include <fmt/format.h>
sympl_namespaces

ScriptVM* ScriptVM::_Instance = nullptr;

ScriptVM::ScriptVM()
{
    __Construct();
}

ScriptVM::~ScriptVM()
{
    if (!IsNullObject(_Instance)) {
        _Instance->Release();
    }
}

void ScriptVM::__Construct()
{
}

void ScriptVM::Startup()
{
    auto globalObject = alloc_ref(ScriptObject);
    globalObject->_Initialize(GLOBAL_SCRIPT_OBJECT, GLOBAL_SCRIPT_OBJECT);
    _Context = alloc_ref(ScriptContext);
    _Context->SetScriptObject(globalObject);

    _MethodRegistry = alloc_ref(MethodRegistry);
    _MethodRegistry->_Initialize();

    _Symbol = alloc_ref(ScriptToken);
}

void ScriptVM::Shutdown()
{
    _Symbol.Release();
    _MethodRegistry.Release();
    _Context.Release();
}

void ScriptVM::GC()
{
}

Interpreter* ScriptVM::LoadFile(const char* filePath)
{
    SharedPtr<ScriptReader> reader = alloc_ref(ScriptReader);
    reader->ReadFile(filePath);

    auto interpret = alloc_ref(Interpreter);
    interpret->_SetReader(reader.Ptr());
    interpret->_Parse();

    return interpret;
}

Interpreter* ScriptVM::LoadString(const char* str)
{
    SharedPtr<ScriptReader> reader = alloc_ref(ScriptReader);
    reader->ReadString(str);

    auto interpret = alloc_ref(Interpreter);
    interpret->_SetReader(reader.Ptr());
    interpret->_Parse();

    return interpret;
}

ScriptObject* ScriptVM::CreateObject(const char* name, ScriptObjectType type, ScriptObject* parent)
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
        _Context->GetScriptObject()->AddChild(scriptObject);
    }

    return scriptObject;
}

ScriptObject* ScriptVM::CreateObject(const char* name, ScriptObject* parent)
{
    return CreateObject(name, ScriptObjectType::Object, parent);
}

bool ScriptVM::AddObject(ScriptObject* scriptObject, ScriptObject* parent)
{
    std::string path = _BuildPath(scriptObject->GetName().c_str(), parent);
    auto* searchObj = FindObjectByPath(path);

    assert(searchObj->IsEmpty() && "Attempted to create duplicate object in the VM!");

    scriptObject->_Initialize(scriptObject->GetName().c_str(), path.c_str(), parent);

    if (!IsNullObject(parent) && !parent->IsEmpty()) {
        parent->AddChild(scriptObject);
    } else {
        _Context->GetScriptObject()->AddChild(scriptObject);
    }

    return true;
}

ScriptObject* ScriptVM::FindObjectByPath(const std::string& path)
{
    std::string delimiter = ".";

    size_t pos = 0;
    std::string parseStr = path;
    std::string token;
    std::string currentPath;

    auto currentObject = &ScriptObject::Empty;
    auto resultObject = &ScriptObject::Empty;

    // Check first if we have the path '.' delimeter. If not.
    // then we should look in the global scope.
    if ((pos = parseStr.find(delimiter)) == std::string::npos) {
        for (auto entryIt : _Context->GetScriptObject()->GetChildren()) {
            if (entryIt->GetPath() == path) {
                resultObject = entryIt.Ptr();
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
                for (auto entryIt : _Context->GetScriptObject()->GetChildren()) {
                    if (entryIt->GetPath() == currentPath) {
                        currentObject = entryIt.Ptr();
                        break;
                    }
                }
                if (currentObject->IsEmpty()) break;
            } else {
                // Search the children.
                currentPath.append(".");
                currentPath.append(token);

                for (auto entryIt : currentObject->GetChildren()) {
                    if (entryIt->GetPath() == currentPath) {
                        currentObject = entryIt.Ptr();
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
                if (entryIt->GetPath() == currentPath) {
                    resultObject = entryIt.Ptr();
                    break;
                }
            }
        }
    }
    return resultObject;
}

ScriptObject* ScriptVM::FindObjectByScope(ScriptObject* scopeObject, const std::string& objectName)
{
    if (!IsNullObject(scopeObject) && !scopeObject->IsEmpty()) {
        return FindObjectByPath(fmt::format("{0}.{1}", scopeObject->GetPath(), objectName));
    }

    return FindObjectByPath(fmt::format("{0}", objectName));
}

std::string ScriptVM::_BuildPath(const char* name, ScriptObject* parent)
{
    if (!IsNullObject(parent)) {
        return fmt::format("{0}.{1}", parent->GetPath(), name);
    }
    return fmt::format("{0}", name);
}

void ScriptVM::RemoveObject(ScriptObject* scriptObject)
{
    if (IsNullObject(scriptObject) || scriptObject->IsEmpty()) {
        return;
    }

    if (scriptObject->GetParent().IsValid()) {
        auto parent = scriptObject->GetParent().Ptr();
        parent->RemoveChild(scriptObject->GetName().c_str());
    }
    scriptObject->Release();
}

void ScriptVM::RemoveObject(const std::string& path)
{
    auto scriptObject = FindObjectByPath(path);
    RemoveObject(scriptObject);
}

MethodRegistry* ScriptVM::GetMethodRegistry()
{
    return _MethodRegistry.Ptr();
}

std::string ScriptVM::PrintObjects()
{
    auto buffer = alloc_ref(StringBuffer);
    for (auto entryIt : _Context->GetScriptObject()->GetChildren()) {
        // Don't print out objects with parents.
        if (entryIt->GetParent().IsValid()) {
            continue;
        }
        buffer->Append(entryIt->Print().c_str());
    }

    std::string results = buffer->CStr();
    free_ref(buffer);

    return results;
}

bool ScriptVM::Release()
{
    Shutdown();
    return true;
}
