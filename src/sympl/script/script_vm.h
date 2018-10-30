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

#include <sympl/core/object.h>
#include <sympl/core/variant.h>
#include <sympl/core/shared_ptr.h>

#include <sympl/script/script_common.h>
#include <sympl/script/method_registry.h>

sympl_nsstart

class MemPool;
class MemPoolRef;
class MemPoolObject;
class MemPoolManager;
class ScriptObject;
class Interpreter;

class SYMPL_API ScriptVM : public Object
{
    SYMPL_OBJECT(ScriptVM, Object);

protected:
    /// Object mapping with given addresses.
    std::unordered_map< MemAddressType, SharedPtr<ScriptObject> > _ObjectMap;

    /// Root reference object.
    SharedPtr<ScriptObject> _GlobalObject;

    /// Reference to the method registry.
    SharedPtr<MethodRegistry> _MethodRegistry;

    //! Constructor.
    ScriptVM();

public:
    //! Destructor.
    ~ScriptVM() override;

    //! Called in place of the constructor.
    void __Construct() override;

    //! Returns the singleton for our class.
    //! \return ScriptVM
    static ScriptVM& GetInstance() {
        static ScriptVM instance;
        instance.SetIsStaticRef(true);

        // Ensure the method registry is initialized.
        instance.GetMethodRegistry();

        return instance;
    }

    //! Creates an object based on a template class.
    //! \tparam T
    //! \param parent
    //! \return
    template<class T>
    T* CreateObject(ScriptObject* parent = nullptr)
    {
        T* ref = mem_alloc_ref(T);

        // There can be only 1 static empty type.
        if (ref->GetType() == ScriptObjectType::Empty) {
            ref->SetType(ScriptObjectType::Object);
        }

        _ObjectMap[ref->GetObjectAddress()] = ref;

        if (parent) {
            parent->AddChild(ref);
        } else {
            _GlobalObject->AddChild(ref);
        }

        return ref;
    }

    //! Creates and initializes an object.
    //! \param name
    //! \param path
    //! \param type
    //! \param parent
    //! \return ScriptObject
    template<class T>
    T* CreateObjectAndInitialize(const std::string& name, ScriptObject* parent = nullptr)
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

        T* ref = CreateObject<T>(parent);
        ref->Initialize(name, path, ref->GetType());
        return ref;
    }

    //! Creates an object with the purpose of being passed by reference.
    //! \return ScriptObject
    ScriptObject* CreateObjectReference();

    //! Attempts to find an object.
    //! \param path
    //! \return ScriptObject
    ScriptObject* FindObjectByPath(const std::string& path);

    //! Loads a file into the vm.
    //! \param filePath
    //! \return Interpreter
    Interpreter* LoadFile(const char* filePath);

    //! Loads a string into the vm.
    //! \param str
    //! \return Interpreter
    Interpreter* LoadString(const char* str);

    //! Returns the object map.
    //! \return
    inline const std::unordered_map< MemAddressType, SharedPtr<ScriptObject> >& GetObjectMap() const { return _ObjectMap; }

    //! Returns the global object.
    //! \return ScriptObject
    inline ScriptObject* GetGlobalObject() const { return _GlobalObject.Ptr(); }

    //! Returns the method registry.
    //! \return SharedPtr<MethodRegistry>
    inline MethodRegistry* GetMethodRegistry() {
        if (!_MethodRegistry.IsValid()) {
            _MethodRegistry = mem_alloc_ref(MethodRegistry);
            _MethodRegistry->_Initialize();
        }
        return _MethodRegistry.Ptr();
    }

public:
    friend MemPool;
    friend MemPoolRef;
    friend MemPoolObject;
    friend MemPoolManager;
};

#define ScriptVMInstance ScriptVM::GetInstance()

sympl_nsend
