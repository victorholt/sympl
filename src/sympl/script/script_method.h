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

sympl_nsstart

enum class MethodReturnType : uint8_t
{
    Object = 0,
    String,
    Int,
    Float,
    Bool
};

struct MethodCallStatement
{
    // TODO: Sharing the Variable reference seems to cause a mem leak...
    WeakRef<ScriptObject> Variable;
    SharedRef<ScriptStatement> Statement;
};

class SYMPL_API ScriptMethod : public ScriptObject
{
    SYMPL_OBJECT(ScriptMethod, ScriptObject);

protected:
    /// Variable paths for the arguments.
    std::vector<WeakRef<ScriptObject>> _Args;

    /// Stored method call statements to execute.
    std::vector<MethodCallStatement*> _CallStatements;

    /// Reference to the scope object.
    WeakRef<ScriptObject> _Scope;

    /// Return type for the method.
    MethodReturnType _ReturnType;

    /// Process the call statements.
    void _ProcessCallStatements();

public:
    //! Constructor.
    ScriptMethod();

    //! Destructor.
    ~ScriptMethod() override;

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
};

#define to_method(obj) static_cast<ScriptMethod*>(obj)

sympl_nsend
