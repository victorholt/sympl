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
#include <sympl/script/script_object.h>

sympl_nsstart

class SYMPL_API ScriptMethod : public ScriptObject
{
protected:
    /// Variable paths for the arguments.
    std::vector<SharedRef<ScriptObject>> _Args;

    /// Object which holds the arguments.
    SharedRef<ScriptObject> _ArgsObject;

    //! Initializes the object.
    //! \param name
    //! \param path
    void _Initialize(const char* name, const char* path, ScriptObject* parent = nullptr) override;

public:
    //! Constructor.
    ScriptMethod();

    //! Destructor.
    ~ScriptMethod() override;

    //! Adds an argument to the method.
    //! \param arg
    void AddArg(ScriptObject* arg);

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

    //! Returns the args script object.
    //! \return SharedRef<ScriptObject>
    inline SharedRef<ScriptObject>& GetArgsObject() {
        return _ArgsObject;
    }
};

#define to_method(obj) static_cast<ScriptMethod*>(obj)

sympl_nsend
