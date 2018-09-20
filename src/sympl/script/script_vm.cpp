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
#include <sympl/script/script_method.h>
#include <sympl/script/script_array.h>

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
    _GlobalObject = mem_alloc_ref(ScriptObject);
    _GlobalObject->_Initialize(GLOBAL_SCRIPT_OBJECT, GLOBAL_SCRIPT_OBJECT);
    _GlobalObject->_SetType(ScriptObjectType::Object);
    _Context = mem_alloc_ref(ScriptContext);
    _Context->SetScriptObject(_GlobalObject);

    _MethodRegistry = mem_alloc_ref(MethodRegistry);
    _MethodRegistry->_Initialize();

    _Symbol = SymplRegistry.Get<ScriptToken>();
}

void ScriptVM::Shutdown()
{
    _Symbol.Release();
    _MethodRegistry.Release();
    _Context.Release();
    mem_free_ref(ScriptObject, _GlobalObject);
}

void ScriptVM::GC()
{
    _UpdateDeleteQueue();
}

Interpreter* ScriptVM::LoadFile(const char* filePath)
{
    SharedPtr<ScriptReader> reader = mem_alloc_ref(ScriptReader);
    reader->ReadFile(filePath);

    if (!_Interpreter.IsValid()) {
        _Interpreter = mem_alloc_ref(Interpreter);
    }
    _Interpreter->_SetReader(reader.Ptr());
    _Interpreter->_Parse();

    return _Interpreter.Ptr();
}

Interpreter* ScriptVM::LoadString(const char* str)
{
    SharedPtr<ScriptReader> reader = mem_alloc_ref(ScriptReader);
    reader->ReadString(str);

    if (!_Interpreter.IsValid()) {
        _Interpreter = mem_alloc_ref(Interpreter);
    }
    _Interpreter->_SetReader(reader.Ptr());
    _Interpreter->_Parse();

    return _Interpreter.Ptr();
}

ScriptObject* ScriptVM::CreateObject(const char* name, ScriptObjectType type, ScriptObject* parent)
{
    std::string path = _BuildPath(name, parent);
    auto* searchObj = FindObjectByPath(
            path,
            nullptr
            //!IsNullObject(parent) && parent->GetParent().IsValid() ? parent->GetParent().Ptr() : nullptr
    );

    if (!searchObj->IsEmpty()) {
        sympl_assert(searchObj->IsEmpty() && "Attempted to create duplicate object in the VM!");
    }

    ScriptObject* scriptObject = nullptr;

    if (type == ScriptObjectType::Method) {
        scriptObject = mem_alloc_ref(ScriptMethod);
    } else if (type == ScriptObjectType::Array) {
        scriptObject = mem_alloc_ref(ScriptArray);
    } else {
        scriptObject = mem_alloc_ref(ScriptObject);
    }
    scriptObject->_SetType(type);
    scriptObject->_Initialize(name, path.c_str(), parent);

    if (!IsNullObject(parent) && !parent->IsEmpty()) {
        parent->AddChild(scriptObject);
    } else {
        _GlobalObject->AddChild(scriptObject);
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
    auto* searchObj = FindObjectByPath(
            path,
            nullptr
            //!IsNullObject(parent) && parent->GetParent().IsValid() ? parent->GetParent().Ptr() : nullptr
    );

    sympl_assert(searchObj->IsEmpty() && "Attempted to create duplicate object in the VM!");

    scriptObject->_Initialize(scriptObject->GetName().c_str(), path.c_str(), parent);

    if (!IsNullObject(parent) && !parent->IsEmpty()) {
        parent->AddChild(scriptObject);
    } else {
        _GlobalObject->AddChild(scriptObject);
    }

    return true;
}

ScriptObject* ScriptVM::FindObjectByPath(const std::string& relPath, ScriptObject* scope)
{
    if (relPath.find("%") != std::string::npos) {
        return &ScriptObject::Empty;
    }

    std::string delimiter = ".";

    size_t pos = 0;

    std::string path = relPath;
    if (!IsNullObject(scope) && !scope->IsEmpty()) {
        path = fmt::format("{0}.{1}", scope->GetPath(), relPath);
    }

    std::string parseStr = path;
    std::string token;
    std::string currentPath;

    auto currentObject = &ScriptObject::Empty;
    auto resultObject = &ScriptObject::Empty;
    auto globalChildren = _GlobalObject->GetChildren();

    // Check first if we have the path '.' delimeter. If not.
    // then we should look in the global scope.
    if ((pos = parseStr.find(delimiter)) == std::string::npos) {
        for (auto& entryIt : globalChildren) {
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
                for (auto& entryIt : globalChildren) {
                    if (entryIt->GetPath() == currentPath) {
                        currentObject = entryIt.Ptr();

                        // Check if we need to assign the current object
                        // to the scope.
                        auto objectType = currentObject->GetType();
                        if (!currentObject->GetChildren().empty() &&
                            (objectType == ScriptObjectType::Object ||
                            objectType == ScriptObjectType::Method ||
                            currentObject->IsClass())) {
                            currentObject = currentObject->GetChildren()[0].Ptr();
                            currentPath.append(".");
                            currentPath.append(SYMPL_SCOPE_NAME);
                        }
                        break;
                    }
                }
                if (currentObject->IsEmpty()) {
                    break;
                }
            } else {
                // Search the children.
                if (token != SYMPL_SCOPE_NAME) {
                    currentPath.append(".");
                    currentPath.append(token);
                }

                for (auto& entryIt : currentObject->GetChildren()) {
                    if (entryIt->GetPath() == currentPath) {
                        currentObject = entryIt.Ptr();

                        // Check if we need to assign the current object
                        // to the scope.
                        auto objectType = currentObject->GetType();
                        if (!currentObject->GetChildren().empty() &&
                            (objectType == ScriptObjectType::Object ||
                             objectType == ScriptObjectType::Method ||
                             currentObject->IsClass())) { // variables could be references to classes.
                            currentObject = currentObject->GetChildren()[0].Ptr();
                            currentPath.append(".");
                            currentPath.append(SYMPL_SCOPE_NAME);
                        } else if (currentObject->HasReferencedValue()) {
                            currentObject = SymplRefRegistry.FindObject(currentObject->GetMeta("RefAddress").GetStringBuffer()->CStr());
                            currentPath = currentObject->GetPath();

                            if (!currentObject->GetChildren().empty() && currentObject->IsClass()) {
                                currentObject = currentObject->GetChildren()[0].Ptr();
                                currentPath.append(".");
                                currentPath.append(SYMPL_SCOPE_NAME);
                            }
                        }

                        break;
                    }
                }
                if (currentObject->IsEmpty()) break;
            }
        }

        // Handle the last part of the path.
        if (!currentObject->IsEmpty()) {
            currentPath.append(".");
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
        return FindObjectByPath(
                fmt::format("{0}.{1}", scopeObject->GetPath(), objectName),
                nullptr
        );
    }

    return FindObjectByPath(fmt::format("{0}", objectName), nullptr);
}

std::string ScriptVM::_BuildPath(const char* name, ScriptObject* parent)
{
    if (!IsNullObject(parent)) {
        return fmt::format("{0}.{1}", parent->GetPath(), name);
    }
    return fmt::format("{0}", name);
}

void ScriptVM::_UpdateDeleteQueue()
{
    if (_DeleteQueue.empty()) {
        return;
    }

    std::vector<ScriptObject*> keep;
    std::vector<ScriptObject*> remove;

    for (auto& entryIt : _DeleteQueue) {
        int numRefs = entryIt->RefCount();
        if (numRefs == 2) {
            remove.push_back(entryIt.Ptr());
        } else {
            keep.push_back(entryIt.Ptr());
        }
    }
    _DeleteQueue.clear();

    for (auto entryIt : keep) {
        _DeleteQueue.push_back(entryIt);
    }
    for (auto entryIt : remove) {
        _RemoveObject(entryIt);
    }
}

void ScriptVM::QueueDelete(ScriptObject* scriptObject)
{
    // Check if we have already queued this object.
    for (auto& entryIt : _DeleteQueue) {
        if (entryIt.Ptr() == scriptObject) {
            return;
        }
    }
    _DeleteQueue.push_back(scriptObject);
}

void ScriptVM::_RemoveObject(ScriptObject* scriptObject)
{
    if (IsNullObject(scriptObject) || scriptObject->IsEmpty()) {
        return;
    }

    if (scriptObject->IsReference()) {
        if (SymplRefRegistry.RemoveAddress(scriptObject->GetReferenceAddress())) {
            scriptObject->Release(); // Release before removing from the global object. THIS MAY BE A BUG! This
                                     // is necessary because the GlobalObject is needed for FindObjectByPath
                                     // to not end up in an infinite loop...
            GetGlobalObject()->RemoveChildByPath(scriptObject->GetPath().c_str());
        }
    } else {
        if (scriptObject->GetParent().IsValid()) {
            auto parent = scriptObject->GetParent().Ptr();
            parent->RemoveChild(scriptObject->GetName().c_str());
        }
    }
}

MethodRegistry* ScriptVM::GetMethodRegistry()
{
    return _MethodRegistry.Ptr();
}

ScriptObject* ScriptVM::GetGlobalObject()
{
    return _GlobalObject;
}

std::string ScriptVM::PrintObjects()
{
    auto buffer = mem_alloc_ref(StringBuffer);
    auto children = _Context->GetScriptObject()->GetChildren();
    for (auto entryIt : children) {
        // Don't print out objects with parents.
        if (entryIt->GetParent().IsValid()) {
            continue;
        }
        buffer->Append(entryIt->Print().c_str());
    }

    std::string results = buffer->CStr();
    mem_free_ref(StringBuffer, buffer);

    return results;
}

std::string ScriptVM::PrintMethods()
{
    SharedPtr<StringBuffer> buffer = mem_alloc_ref(StringBuffer);
    auto methods = _MethodRegistry->GetMethods();
    for (auto method : methods) {
        buffer->Append(method.first);
        buffer->Append(" (");
        buffer->Append(method.second->GetCleanName());
        buffer->Append(": ");
        buffer->Append(method.second->GetPath());
        buffer->Append(")");
        buffer->Append("\n");
    }

    std::string results = buffer->CStr();
    return results;
}

bool ScriptVM::Release()
{
    Shutdown();
    return true;
}
