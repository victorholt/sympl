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
#include <sympl/core/shared_ref.h>
#include <sympl/core/weak_ref.h>
#include <sympl/script/script_object.h>
#include <sympl/core/string_buffer.h>

sympl_nsstart

enum class MethodReturnType : uint8_t
{
    Object = 0,
    String,
    Int,
    Float,
    Bool,
    Void
};

struct MethodCallStatement
{
    // TODO: Sharing the Variable reference seems to cause a mem leak...
    SharedRef<ScriptObject> Variable;
    SharedRef<ScriptStatement> Statement;
};

class SYMPL_API ScriptMethod : public ScriptObject
{
    SYMPL_OBJECT(ScriptMethod, ScriptObject);

protected:
    /// Variable paths for the arguments.
    std::vector<SharedRef<ScriptObject>> _Args;

    /// Stored method call statements to execute.
    std::vector<MethodCallStatement*> _CallStatements;

    /// Reference to the scope object.
    SharedRef<ScriptObject> _Scope;

    /// Return type for the method.
    MethodReturnType _ReturnType;

    /// Arguments given to the method as a string.
    SharedRef<StringBuffer> _ArgString;

    /// Flag to determine whether this is an immediate method.
    /// Immediate methods are called the moment they are defined (if, while, etc).
    bool _IsImmediate = false;

    /// Flag to ignore the return type check.
    bool _IgnoreReturnTypeCheck = false;

    /// Flag to exit the method.
    bool _Exit = false;

    //! Copy over argument values from a list of arguments.
    //! \param args
    virtual void _CopyArgs(const std::vector<Variant>& args);

    //! Process the argument statements.
    virtual void _ProcessArgStatements();

    //! Process the call statements.
    virtual void _ProcessCallStatements();

    //! Checks the return type based on the given variant value.
    //! \param value
    bool _CheckReturnType(const Variant& value);

public:
    //! Constructor.
    ScriptMethod();

    //! Evaluates and returns the results of the object.
    //! \param args
    //! \return
    Variant Evaluate(const std::vector<Variant>& args) override;

    //! Evaluates and returns the results of the object.
    //! \return
    Variant Evaluate() override;

    //! Adds an argument to the method.
    //! \param arg
    void AddArg(ScriptObject* arg);

    //! Adds a statement for the method to call.
    //! \param variable
    //! \param stat
    void AddStatement(ScriptObject* variable, ScriptStatement* stat);

    //! Creates a clone of the object.
    //! \param parent
    //! \param uniqueName
    //! \return SharedRef<ScriptObject>
    ScriptObject* Clone(ScriptObject* parent, bool uniqueName) override;

    //! Returns the scope object.
    //! \return ScriptObject
    ScriptObject* GetScope();

    //! Returns the parent of the current scope.
    //! \return ScriptObject.
    ScriptObject* GetScopeParent();

    //! Sets the signal exit flag.
    void Exit();

    //! Releases the object.
    void Release() override;

    //! Sets the exit flag.
    //! \param exit
    inline void SetExit(bool exit) { _Exit = exit; }

    //! Returns the argument object.
    //! \param index
    //! \return SharedRef<ScriptObject>
    inline ScriptObject* GetArg(unsigned index = 0) {
        if (index > _Args.size()) {
            return &ScriptObject::Empty;
        }
        return _Args[index].Ptr();
    }

    //! Returns the number of arguments this method takes.
    inline unsigned GetNumPotentialArgs() const {
        return static_cast<unsigned>(_Args.size());
    }

    //! Returns the number of arguments given to this method.
    inline unsigned GetNumArgs() const {
        return 0;
    }

    //! Sets the return type.
    //! \param type
    inline void SetReturnType(MethodReturnType type) {
        _ReturnType = type;
    }

    //! Returns the return type.
    //! \return MethodReturnType
    inline MethodReturnType GetReturnType() const {
        return _ReturnType;
    }

    //! Sets whether or not the method is immediate.
    //! \param immediate.
    inline void SetImmediate(bool immediate) {
        _IsImmediate = immediate;
    }

    //! Returns whether or not the method is immediate.
    //! \return bool
    inline bool IsImmediate() const {
        return _IsImmediate;
    }

    //! Sets the arguments string.
    //! \param argString
    inline void SetArgString(StringBuffer* argString) {
        SetArgString(argString->CStr());
    }

    //! Sets the arguments string.
    //! \param argString
    inline void SetArgString(const char* argString) {
        _ArgString->Clear();
        _ArgString->Append(argString);
    }

    //! Returns the argument string.
    //! \return string
    inline StringBuffer* GetArgString() const {
        return _ArgString.Ptr();
    }

    //! Sets the ignore return type check flag.
    //! \param ignore
    inline void SetIgnoreReturnTypeCheck(bool ignore) {
        _IgnoreReturnTypeCheck = ignore;
    }

    //! Returns whether or not to ignore type checking.
    //! \return bool
    inline bool GetIgnoreReturnTypeCheck() const {
        return _IgnoreReturnTypeCheck;
    }
};

#define to_method(obj) static_cast<ScriptMethod*>(obj)

sympl_nsend
