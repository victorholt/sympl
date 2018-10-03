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
#include <sympl/script/script_common.h>
#include <sympl/script/resolvers/object_resolver.h>

sympl_nsstart

class SYMPL_API ArrayResolver : public ObjectResolver
{
    SYMPL_OBJECT(ArrayResolver, ObjectResolver);

protected:
    //! Checks if this is a value assignment and returns the assignment address.
    //! \param index
    //! \return
    bool _CheckValueAssignment(StatementResolver* stmtResolver, ScriptObject* varObject, Variant& index);

    //! Attempts to resolve an array.
    //! \param stmtResolver
    //! \param currentStr
    //! \param destObject
    //! \param op
    //! \return Variant
    Variant _ResolveArray(StatementResolver* stmtResolver, StringBuffer* currentStr,
                               ScriptObject* destObject, StatementOperator op);

    //! Attempts to resolve an array value.
    //! \param stmtResolver
    //! \param currentStr
    //! \param destObject
    //! \param op
    //! \return Variant
    Variant _ResolveArrayValue(StatementResolver* stmtResolver, StringBuffer* currentStr,
                         ScriptObject* destObject, StatementOperator op);

    //! Attempts to modify an existing array.
    //! \param stmtResolver
    //! \param currentStr
    //! \param destObject
    //! \param op
    //! \return Variant
    Variant _ModifyExistingArray(StatementResolver* stmtResolver, StringBuffer* currentStr,
                                 ScriptObject* destObject, StatementOperator op);

public:
    ArrayResolver() = default;

    //! Called in place of the constructor.
    void __Construct() override {}

    //! Resolves the array.
    //! \param stmtResolver
    //! \param currentStr
    //! \param destObject
    //! \param op
    //! \return
    virtual Variant Resolve(StatementResolver* stmtResolver, StringBuffer* currentStr,
                            ScriptObject* destObject, StatementOperator op);
};

sympl_nsend