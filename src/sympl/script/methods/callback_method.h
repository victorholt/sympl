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

#include <sympl/script/script_method.h>

sympl_nsstart

class SYMPL_API CallbackMethod : public ScriptMethod
{
    SYMPL_OBJECT(CallbackMethod, ScriptMethod);

protected:
    /// Callback method called during the evaluate process.
    SymplMethodCallback _Callback;

    //! Initializes the object.
    //! \param name
    //! \param path
    void _Initialize(const char* name, const char* path, ScriptObject* parent = nullptr) override;

public:
    //! Constructor.
    CallbackMethod();

    //! Evaluates and returns the results of the object.
    //! \param args
    //! \return
    Variant Evaluate(const std::vector<Variant>& args) override;

    //! Sets the callback for the method.
    //! \param callback
    inline void SetCallback(SymplMethodCallback callback) { _Callback = callback; }
};

sympl_nsend
