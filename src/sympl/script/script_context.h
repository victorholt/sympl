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
#include <sympl/core/object_ref.h>
#include <sympl/core/shared_ptr.h>
#include <sympl/core/weak_ptr.h>

sympl_nsstart

class ScriptObject;

class SYMPL_API ScriptContext : public ObjectRef
{
SYMPL_OBJECT(ScriptContext, ObjectRef);

protected:
    /// Owner of the context.
    WeakPtr<ScriptObject> _Owner;

    /// Reference to the current scope of the context.
    WeakPtr<ScriptObject> _CurrentScope;

    /// Reference to the caller.
    WeakPtr<ScriptObject> _Caller;

    /// Reference to the previous context.
    WeakPtr<ScriptContext> _PreviousContext;

    /// Reference to the next context.
    WeakPtr<ScriptContext> _NextContext;

    /// Quick variable access list.
    std::vector<WeakPtr<ScriptObject>> _VarList;

    /// Current return value.
    Variant _RetValue;

    /// Flag to signal an exit.
    bool _Exit = false;

public:
    //! Constructor.
    ScriptContext();

    //! Destructor.
    ~ScriptContext() override;

    //! Finds an object within the scope.
    //! \return ScriptObject
    ScriptObject* FindObject(const char* name);

    //! Sets the owner script object.
    //! \param owner
    void SetOwner(ScriptObject* owner);

    //! Returns the owner script object.
    //! \return ScriptObject
    ScriptObject* GetOwner() const;

    //! Sets the current scope script object.
    //! \param scope
    void SetCurrentScope(ScriptObject* scope);

    //! Returns the current scope script object.
    //! \return ScriptObject
    ScriptObject* GetCurrentScope() const;

    //! Sets the caller script object.
    //! \param caller
    void SetCaller(ScriptObject* caller);

    //! Returns the caller script object.
    //! \return ScriptObject
    ScriptObject* GetCaller() const;

    //! Sets the previous context.
    //! \param context
    inline void SetPreviousContext(ScriptContext* context) { _PreviousContext = context; }

    //! Returns the previous context.
    //! \return ScriptContext
    inline ScriptContext* GetPreviousContext() const { return _PreviousContext.Ptr(); }

    //! Sets the next context.
    //! \param context
    inline void SetNextContext(ScriptContext* context) { _NextContext = context; }

    //! Returns the next context.
    //! \return ScriptContext
    inline ScriptContext* GetNextContext() const { return _NextContext.Ptr(); }

    //! Sets the return value.
    //! \param value
    inline void SetReturnValue(const Variant& value) { _RetValue = value; }

    //! Returns the return value.
    //! \return Variant
    inline Variant GetReturnValue() const { return _RetValue; }

    //! Sets the exit flag.
    //! \param exit
    void Exit();

    //! Returns the exit flag.
    //! \return bool
    inline bool GetExit() const { return _Exit; }

    //! Check for empty of this context.
    //! \return bool
    inline bool IsEmpty() const { return !_Owner.IsValid(); }

    /// Empty script context.
    static ScriptContext Empty;
};

sympl_nsend
