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
#include <sympl/script/script_vm.h>
#include <sympl/script/script_object.h>
#include <sympl/script/script_array.h>
#include <sympl/script/script_reader.h>
#include <sympl/script/interpreter.h>

#include <sympl/util/string_helper.h>
sympl_namespaces

ScriptVM::ScriptVM()
{
    __Construct();
}

ScriptVM::~ScriptVM()
{
}

void ScriptVM::__Construct()
{
    _BuildAddresses();

    // Create the global object.
    _GlobalObject = mem_alloc_ref(ScriptObject);
    _GlobalObject->SetObjectAddress(ReserveObjectAddress());
    _ObjectMap[_GlobalObject->GetObjectAddress()] = _GlobalObject;
    _GlobalObject->Initialize("root", "root", ScriptObjectType::Object);
}

ScriptObject* ScriptVM::CreateObject(ScriptObject* parent)
{
    return CreateObject(ScriptObjectType::Object, parent);
}

ScriptObject* ScriptVM::CreateObject(ScriptObjectType type, ScriptObject* parent)
{
    ScriptObject* ref;

    if (type == ScriptObjectType::Array) {
        ref = mem_alloc_ref(ScriptArray);
    } else {
        ref = mem_alloc_ref(ScriptObject);
    }

    ref->SetObjectAddress(ReserveObjectAddress());
    _ObjectMap[ref->GetObjectAddress()] = ref;

    if (parent) {
        parent->AddChild(ref);
    } else {
        _GlobalObject->AddChild(ref);
    }

    return ref;
}

ScriptObject* ScriptVM::CreateObjectAndInitialize(const std::string& name, ScriptObjectType type, ScriptObject* parent)
{
    std::string path;
    if (parent) {
        path = fmt::format("{0}.{1}", parent->GetPath(), name);
    } else {
        path = fmt::format("{0}", name);
    }

    // Check for collision.
    auto searchObj = FindObjectByPath(path);
    sympl_assert(searchObj->IsEmpty(), "Collision detected with paths in an attempt to create a new object!");

    auto ref = CreateObject(type, parent);
    ref->Initialize(name, path, type);
    return ref;
}

ScriptObject* ScriptVM::CreateObjectReference()
{
    auto ref = CreateObject(ScriptObjectType::Object);
    ref->Initialize(ref->GetObjectAddress(), ref->GetObjectAddress(), ScriptObjectType::Object);
    return ref;
}

void ScriptVM::_BuildAddresses()
{
    _ObjectMap.clear();
    _ObjectMap.reserve(_MaxObjectAddresses);

    _AvailableObjectAddresses.clear();
    _AvailableObjectAddresses.reserve(_MaxObjectAddresses);

    for (size_t i = 0; i < _MaxObjectAddresses; i++) {
        auto address = StringHelper::GenerateRandomStr(64);
        _AvailableObjectAddresses.push_back(address);
    }
}

std::string ScriptVM::ReserveObjectAddress()
{
    auto result = _AvailableObjectAddresses.back();
    _AvailableObjectAddresses.pop_back();
    return result;
}

void ScriptVM::ReleaseObjectAddress(const std::string& address)
{
    sympl_assert(IsAvailableObjectAddress(address), "Attempted to release an object address that's already available!");
    _ObjectMap.erase(address);
    _AvailableObjectAddresses.push_back(address);
}

bool ScriptVM::IsAvailableObjectAddress(const std::string& address)
{
    auto addressIt = std::begin(_AvailableObjectAddresses);
    while (addressIt != std::end(_AvailableObjectAddresses)) {
        if (*addressIt == address) {
            return false;
        }
        ++addressIt;
    }
    return true;
}

ScriptObject* ScriptVM::FindObjectByPath(const std::string& path)
{
    if (path.find("%") != std::string::npos) {
        return &ScriptObject::Empty;
    }

    std::string delimiter = ".";

    size_t pos = 0;

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
                             objectType == ScriptObjectType::Method)
                         ) {
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
                             objectType == ScriptObjectType::Method)
                         ) { // variables could be references to classes.
                            currentObject = currentObject->GetChildren()[0].Ptr();
                            currentPath.append(".");
                            currentPath.append(SYMPL_SCOPE_NAME);
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

Interpreter* ScriptVM::LoadFile(const char* filePath)
{
    SharedPtr<Interpreter> interpreter = mem_alloc_ref(Interpreter);
    SharedPtr<ScriptReader> reader = mem_alloc_ref(ScriptReader);
    sympl_assert(reader->ReadFile(filePath), "Failed to find import file!");

    if (!interpreter.IsValid()) {
        interpreter = mem_alloc_ref(Interpreter);
    }
    interpreter->_SetReader(reader.Ptr());
    interpreter->_Parse();

    return interpreter.Ptr();
}

Interpreter* ScriptVM::LoadString(const char* str)
{
    SharedPtr<Interpreter> interpreter = mem_alloc_ref(Interpreter);
    SharedPtr<ScriptReader> reader = mem_alloc_ref(ScriptReader);
    reader->ReadString(str);

    if (!interpreter.IsValid()) {
        interpreter = mem_alloc_ref(Interpreter);
    }
    interpreter->_SetReader(reader.Ptr());
    interpreter->_Parse();

    return interpreter.Ptr();
}
