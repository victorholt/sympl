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
#include <sympl/core/string_buffer.h>

#include <sympl/script/script_object.h>

sympl_nsstart

enum class StatementOperator {
    None = 0,
    Equals,
    Add,
    Subtract,
    Divide,
    Multiply,
    Mod,
    GreaterThan,
    LessThan,
    GreaterEqualThan,
    LessEqualThan
};

enum class StatementType {
    None = 0,
    Object,
    String,
    Bool,
    Integer,
    Float
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

    /// Representation of the statement as a string.
    StringBuffer* _String;

    /// Type of statement.
    StatementType _Type;

    //! Finds the type for a variant.
    //! \param value
    //! \return StatementType
    StatementType _FindType(const Variant& value);

    //! Applies the entry to the current value.
    //! \param entry
    //! \param value
    void _Apply(StatementObjectEntry* entry, Variant& value);

public:
    //! Constructor.
    ScriptStatement() {
        _Type = StatementType::None;
        _String = alloc_ref(StringBuffer);
        _String->Resize(512);
    }

    //! Destructor.
    ~ScriptStatement() override {
        for (auto entryIt : _Entries) {
            entryIt->ConstantValue.Clear();
            delete entryIt;
        }
        _Entries.clear();

        free_ref(StringBuffer, _String);
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

    //! Returns evaluate value as a string.
    //! \return string
    std::string EvaluateAsString();

    //! Returns the string.
    //! \return StringBuffer
    inline StringBuffer* GetString() const { return _String; }

    //! Sets the type for the statement.
    //! \param type
    inline void SetType(StatementType type) { _Type = type; }

    //! Returns the type for this statement.
    //! \return StatementType
    inline StatementType GetType() const { return _Type; }

    //! Returns the type as a string
    //! \return string
    std::string GetTypeAsString() const;
};

sympl_nsend
