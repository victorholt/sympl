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

#include <sympl/script/script_object.h>

sympl_nsstart

enum class StatementOperator {
    Equals = 0,
    Plus,
    Minus,
    Divide,
    Multiply,
    Mod,
    GreaterThan,
    LessThan,
    GreaterEqualThan,
    LessEqualThan
};

/// Entry for the objects in the statement.
struct StatementObjectEntry {
    StatementOperator Op;
    SharedRef<ScriptObject> Value;
    Variant ConstantValue;

    // Next statement entry to resolve before we consider
    // the current statement entry complete.
    StatementObjectEntry* Next = nullptr;
};

class SYMPL_API ScriptStatement : public Object
{
private:
    /// Script objects that make up the statement.
    std::vector<StatementObjectEntry*> _Entries;

public:
    //! Constructor.
    ScriptStatement() {}

    //! Destructor.
    ~ScriptStatement() override {
        for (auto entryIt : _Entries) {
            delete entryIt;
        }
        _Entries.clear();
    }

    //! Adds a script object as part of the statement.
    //! \param scriptObject
    //! \param op
    StatementObjectEntry* Add(ScriptObject*& scriptObject, StatementOperator op = StatementOperator::Equals);

    //! Adds a constant value as part of the statement.
    //! \param constantValue
    //! \param op
    StatementObjectEntry* Add(const Variant& constantValue, StatementOperator op = StatementOperator::Equals);

    //! Evaluates the statement.
    //! \return Variant
    const Variant& Evaluate();
};

sympl_nsend
