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
    /// Max available addresses.
    size_t _MaxObjectAddresses = 100000;

    /// Available object addresses.
    std::vector<std::string> _AvailableObjectAddresses;

    /// Object mapping with given addresses.
    std::unordered_map< std::string, SharedPtr<ScriptObject> > _ObjectMap;

    /// Root reference object.
    SharedPtr<ScriptObject> _GlobalObject;

    //! Build the available object addresses.
    void _BuildAddresses();

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
        return instance;
    }

    //! Creates an empty object and attaches it to the global object.
    //! \return ScriptObject
    ScriptObject* CreateObject(ScriptObject* parent = nullptr);

    //! Creates an empty object and attaches it to the global object.
    //! \return type
    //! \return ScriptObject
    ScriptObject* CreateObject(ScriptObjectType type, ScriptObject* parent = nullptr);

    //! Creates and initializes an object.
    //! \param name
    //! \param path
    //! \param type
    //! \param parent
    //! \return ScriptObject
    ScriptObject* CreateObjectAndInitialize(const std::string& name, ScriptObjectType type = ScriptObjectType::Object, ScriptObject* parent = nullptr);

    //! Creates an object with the purpose of being passed by reference.
    //! \return ScriptObject
    ScriptObject* CreateObjectReference();

    //! Reserves an object address.
    //! \return std::string
    std::string ReserveObjectAddress();

    //! Releases an object address.
    //! \param address
    void ReleaseObjectAddress(const std::string& address);

    //! Checks if an address is available.
    //! \param address
    //! \return bool
    bool IsAvailableObjectAddress(const std::string& address);

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

    //! Sets the max number object addresses.
    //! \param value
    inline void SetMaxObjectAddresses(size_t value) { _MaxObjectAddresses = value; }

    //! Returns the max number object addresses.
    //! \return
    inline size_t GetMaxObjectAddresses() const { return _MaxObjectAddresses; }

    //! Returns the object map.
    //! \return
    inline const std::unordered_map< std::string, SharedPtr<ScriptObject> >& GetObjectMap() const { return _ObjectMap; }

    //! Returns the global object.
    //! \return ScriptObject
    inline ScriptObject* GetGlobalObject() const { return _GlobalObject.Ptr(); }

public:
    friend MemPool;
    friend MemPoolRef;
    friend MemPoolObject;
    friend MemPoolManager;
};

#define ScriptVMInstance ScriptVM::GetInstance()

sympl_nsend
