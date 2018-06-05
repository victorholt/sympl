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

#include <sympl/core/variant.h>
#include <sympl/core/object.h>
#include <sympl/core/shared_ptr.h>
#include <sympl/core/weak_ptr.h>

#include <sympl/thirdparty/urho3d/container/Vector.h>

sympl_nsstart

class ScriptObject;

class SYMPL_API ScriptContext : public Object
{
    SYMPL_OBJECT(ScriptContext, Object);

protected:
    /// Owner of the context.
    SharedPtr<ScriptObject> _Object;

    /// Reference to the parent context.
    WeakPtr<ScriptContext> _ParentContext;

    /// Reference to the caller context.
    /// This is set for methods.
    WeakPtr<ScriptContext> _CallerContext;

    /// Quick variable access list.
    Urho3D::PODVector<ScriptObject*> _VarList;

    /// Current return value.
    Variant _RetValue;

    /// Flag to signal an exit.
    bool _Exit = false;

public:
    //! Constructor.
    ScriptContext();

    //! Called in place of the constructor.
    void __Construct() override;

    //! Adds a variable to the context.
    //! \param varObject
    void AddVar(ScriptObject* varObject);

    //! Finds an object within the scope.
    //! \param name
    //! \param traverse
    //! \return ScriptObject
    ScriptObject* FindVariable(const char* name, bool traverse = true);

    //! Attempts to remove a variable.
    //! \param scriptObject
    void RemoveVariable(ScriptObject* scriptObject);

    //! Copy our variable list for easy access.
    //! \param context
    void CopyVarsTo(ScriptContext* context);

    //! Sets the owner of the context.
    //! \param scriptObject
    void SetScriptObject(ScriptObject* scriptObject);

    //! Returns the script object.
    //! \return ScriptObject
    ScriptObject* GetScriptObject();

    //! Releases the object.
    bool Release() override;

    //! Sets the parent context.
    //! \param context
    inline void SetParentContext(ScriptContext* context) { _ParentContext = context; }

    //! Returns the parent context.
    //! \return
    inline ScriptContext* GetParentContext() const { return _ParentContext.IsValid() ? _ParentContext.Ptr() : &ScriptContext::Empty; }

    //! Sets the caller context.
    //! \param context
    inline void SetCallerContext(ScriptContext* context) { _CallerContext = context; }

    //! Returns the caller context.
    //! \return
    inline ScriptContext* GetCallerContext() const { return _CallerContext.IsValid() ? _CallerContext.Ptr() : &ScriptContext::Empty; }

    //! Check for empty of this context.
    //! \return bool
    inline bool IsEmpty() const { return !_Object.IsValid(); }

    /// Empty script context.
    static ScriptContext Empty;
};

sympl_nsend
