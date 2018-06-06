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
#include <sympl/core/shared_ptr.h>
#include <sympl/core/weak_ptr.h>
#include <sympl/core/string_buffer.h>

#include <sympl/script/script_token.h>

#include <sympl/script/script_object.h>
#include <sympl/script/script_context.h>

#include <sympl/thirdparty/urho3d/container/Vector.h>

sympl_nsstart

struct StatementEntry
{
    SharedPtr<ScriptObject> EvalObject;
    Variant ConstantValue;
    std::string EvalString;
};

class SYMPL_API StatementResolver : public Object
{
    SYMPL_OBJECT(StatementResolver, Object);

private:
    /// Statements to resolve. Resolves the last first.
    Urho3D::PODVector<StatementEntry> _Statements;

    /// Statement string we want to resolve.
    StringBuffer* _StmtString = nullptr;

    /// Current character location.
    size_t _CurrentCharLocation = 0;

    //! Checks if a given string is an object.
    //! \param str
    //! \return
    ScriptObject* _IsObject(const char* str);

    //! Checks if a given string is a number.
    //! \param str
    //! \return bool
    Variant _IsNumber(const char* str);

public:
    //! Constructor.
    StatementResolver();

    //! Called in place of the constructor.
    void __Construct() override;

    //! Attempts to resolve a given string.
    //! \param str
    void Resolve(const char* str);
};

sympl_nsend
