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
#pragma once

#include <sympl/core/sympl_pch.h>
#include <sympl/core/object_ref.h>
#include <sympl/core/variant.h>
#include <sympl/core/shared_ptr.h>
#include <sympl/script/script_common.h>
#include <sympl/script/script_context.h>

sympl_nsstart

class Interpreter;
class MethodRegistry;
class AllocManager;

class SYMPL_API ScriptVM : public ObjectRef
{
SYMPL_OBJECT(ScriptVM, ObjectRef);

private:
    /// Instance for the vm.
    static ScriptVM* _Instance;

    /// Reference to the method registry.
    SharedPtr<MethodRegistry> _MethodRegistry;

    /// The primary scope containing the global variables.
    SharedPtr<ScriptContext> _Context;

    //! Builds a path based on the given name/parent.
    //! \param name
    //! \param parent
    //! \return
    std::string _BuildPath(const char* name, ScriptObject* parent);

    //! Constructor.
    ScriptVM();

public:
    //! Destructor.
    ~ScriptVM() override;

    //! Called in place of the constructor.
    void __Construct() override;

    //! Returns the instance.
    //! \return ScriptVM
    static ScriptVM* GetInstance() {
        if (IsNullObject(_Instance)) {
            _Instance = alloc_ref(ScriptVM);
            _Instance->Startup();
        }
        return _Instance;
    }

    //! Starts up the vm.
    void Startup();

    //! Shuts down the vm.
    void Shutdown();

    //! Attempts to garbage collect objects.
    void GC();

    //! Loads a file into the vm.
    //! \param filePath
    //! \return Interpreter
    Interpreter* LoadFile(const char* filePath);

    //! Loads a string into the vm.
    //! \param str
    //! \return Interpreter
    Interpreter* LoadString(const char* str);

    //! Creates a new script object.
    //! \param name
    //! \param parent
    //! \return
    ScriptObject* CreateObject(const char* name, ScriptObjectType type, ScriptObject* parent = nullptr);

    //! Creates a new script object.
    //! \param name
    //! \param parent
    //! \return
    ScriptObject* CreateObject(const char* name, ScriptObject* parent = nullptr);

    //! Adds an object to the VM.
    //! \param scriptObject
    //! \param parent
    //! \return bool
    bool AddObject(ScriptObject* scriptObject, ScriptObject* parent = nullptr);

    //! Attempts to find an object.
    //! \param path
    //! \return ScriptObject
    ScriptObject* FindObjectByPath(const std::string& path);

    //! Attempts to find an object with a given scope.
    //! \param scopeObject
    //! \param objectName
    //! \return ScriptObject
    ScriptObject* FindObjectByScope(ScriptObject* scopeObject, const std::string& objectName);

    //! Remove object from the object map.
    //! \param scriptObject
    void RemoveObject(ScriptObject* scriptObject);

    //! Remove object from the object map.
    //! \param path
    void RemoveObject(const std::string& path);

    //! Returns the method registry.
    //! \return SharedPtr<MethodRegistry>
    MethodRegistry* GetMethodRegistry();

    //! Returns a string representing objects in the vm.
    //! \return string
    std::string PrintObjects();

    //! Releases the object.
    bool Release() override;

    //! Returns the main context.
    //! \return ScriptContext
    inline ScriptContext* GetContext() const { return _Context.Ptr(); }

    friend AllocManager;
};

#define ScriptVMInstance Sympl::ScriptVM::GetInstance()

sympl_nsend