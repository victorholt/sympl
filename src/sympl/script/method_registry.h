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
#include <sympl/core/weak_ptr.h>

#include <sympl/script/script_method.h>

sympl_nsstart

class ScriptVM;

// Simple registry for holding GLOBAL methods used
// in programs.

class SYMPL_API MethodRegistry : public ObjectRef
{
SYMPL_OBJECT(MethodRegistry, ObjectRef);

private:
    /// List of methods available.
    std::unordered_map<std::string, SharedPtr<ScriptMethod> >_Methods;

    //! Initializes the system methods.
    void _Initialize();

public:
    //! Constructor.
    MethodRegistry();

    //! Destructor.
    ~MethodRegistry() override;

    //! Called in place of the constructor.
    void __Construct() override;

    //! Adds a method to the registry.
    //! \param method
    //! \param handle
    void AddMethod(ScriptObject* method);

    //! Adds a callback method.
    //! \param name
    //! \param callback
    //! \param returnType
    void AddCallbackMethod(const char* name, const SymplMethodCallback& callback, MethodReturnType returnType = MethodReturnType::Void);

    //! Finds and returns a method.
    //! \param name
    //! \return ScriptMethod
    ScriptObject* FindMethod(const char* name);

    //! Returns whether a method was found.
    //! \param name
    //! \param method
    //! \return bool
    bool TryFindMethod(const char* name, ScriptObject*& method);

    //! Releases the object.
    bool Release() override;

    // ScriptVM is a friend to this class.
    friend ScriptVM;
};

sympl_nsend
