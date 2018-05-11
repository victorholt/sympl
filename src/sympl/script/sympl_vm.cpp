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
#include <sympl/script/sympl_vm.h>

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

}

void SymplVM::Shutdown()
{
    GC();
    _ObjectMap.clear();
}

void SymplVM::GC()
{
    for (auto entryIt : _ObjectMap) {
        if (!entryIt.second.IsValid()) {
            _ObjectMap.erase(entryIt.first);
        }
    }
}

ScriptObject* SymplVM::CreateObject(const char* name, ScriptObjectType type, ScriptObject* parent)
{
    std::string path = _BuildPath(name, parent);
    auto* searchObj = FindObject(path);
    if (!searchObj->IsEmpty()) {
        return searchObj;
    }

    SharedRef<ScriptObject> scriptObject = alloc_ref(ScriptObject);
    scriptObject->_Initialize(name, path.c_str(), parent);
    scriptObject->_SetType(type);
    _ObjectMap[scriptObject->GetPath()] = scriptObject;

    if (!IsNullObject(parent)) {
        parent->_AddChild(scriptObject.Ptr());
    }

    return scriptObject.Ptr();
}

ScriptObject* SymplVM::CreateObject(const char* name, ScriptObject* parent)
{
    return CreateObject(name, ScriptObjectType::Object, parent);
}

ScriptObject* SymplVM::FindObject(const std::string& path)
{
    auto entryIt = _ObjectMap.find(path);
    if (entryIt == _ObjectMap.end()) {
        return &ScriptObject::Empty;
    }

    return entryIt->second.Ptr();
}

std::string SymplVM::_BuildPath(const char* name, ScriptObject* parent)
{
    if (!IsNullObject(parent)) {
        return fmt::format(".{0}.{1}", parent->GetPath(), name);
    }
    return fmt::format(".{0}", name);
}

std::string SymplVM::PrintObjects()
{
    auto buffer = alloc_ref(StringBuffer);
    for (auto entryIt : _ObjectMap) {
        buffer->Append(entryIt.second->Print().c_str());
    }

    std::string results = buffer->CStr();
    free_ref(StringBuffer, buffer);

    return results;
}
