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
#include <sympl/core/sympl_object.h>
#include <sympl/core/shared_ref.h>
#include <sympl/script/script_common.h>
#include <sympl/script/script_object.h>
#include <sympl/script/script_method.h>

sympl_nsstart

class SYMPL_API SymplVM : public Object
{
private:
    /// Instance for the profiler.
    static SymplVM* _Instance;

    /// Map of all objects in the vm.
    std::unordered_map<std::string, SharedRef<ScriptObject>> _ObjectMap;

    //! Builds a path for an object.
    //! \param name
    //! \param parent
    //! \return string
    std::string _BuildPath(const char* name, ScriptObject* parent = nullptr);

    //! Constructor.
    SymplVM();

public:

    //! Destructor.
    virtual ~SymplVM();

    //! Returns the instance.
    //! \return Profiler
    static SymplVM* GetInstance() {
        if (IsNullObject(_Instance)) {
            _Instance = alloc_ref(SymplVM);
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

    //! Attempts to find an object.
    //! \param path
    //! \return ScriptObject
    ScriptObject* FindObject(const std::string& path);

    //! Returns a string representing objects in the vm.
    //! \return string
    std::string PrintObjects();

    /// Ensure the alloc class can access this object properly.
    friend Alloc;
};

#define SymplVMInstance Sympl::SymplVM::GetInstance()

sympl_nsend
