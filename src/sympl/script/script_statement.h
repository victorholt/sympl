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

sympl_nsstart

enum class StatementOperator {
    None = 0,
    Equals,
    Add,
    Subtract,
    Divide,
    Multiply,
    Mod,
    IsEqual2,
    NotIsEqual2,
    GreaterThan,
    LessThan,
    GreaterEqualThan,
    LessEqualThan
};

enum class StatementType {
    None = 0,
    Object,
    Method,
    String,
    Bool,
    Integer,
    Float,
    Void
};

/// Entry for the objects in the statement.
struct StatementObjectEntry {
    StatementOperator Op;
    WeakPtr<ScriptObject> Value;
    Variant ConstantValue;
    StatementObjectEntry* NextEntry;
};

class SYMPL_API ScriptStatement : public Object
{
    SYMPL_OBJECT(ScriptStatement, Object);

private:
    /// Script objects that make up the statement.
    StatementObjectEntry* _FirstEntry;
    /// The current entry.
    StatementObjectEntry* _CurrentEntry;

    /// Representation of the statement as a string.
    SharedPtr<StringBuffer> _String;

    /// Symbol token checker.
    ScriptToken* _Symbol;

    /// Type of statement.
    StatementType _Type;

    /// The current character location of the statement we're building.
    unsigned _CurrentCharLocation = 0;

    //! Finds the type for a variant.
    //! \param value
    //! \return StatementType
    StatementType _FindType(const Variant& value);

    //! Statement buffer for building out the statements.
    StringBuffer* _StatementBuffer;

    //! Resolves statements within a parenthesis and returns a value.
    //! \param varObject
    //! \param statementStr
    //! \param op
    //! \return Variant
    Variant _ResolveParenth(ScriptObject* varObject, StringBuffer* statementStr, StatementOperator op);

    //! Resolves a method in a given statement to a value.
    //! \param varObject
    //! \param statementStr
    //! \param op
    Variant _ResolveMethod(ScriptObject* varObject, StringBuffer* statementStr, StatementOperator op);

    //! Returns an evaluation from the current statement buffer.
    //! \param scriptObject
    //! \return Variant
    Variant GetEvalFromStatementBuffer(ScriptObject* scriptObject);

    //! Add a value operation to the statement.
    //! \param value
    //! \param op
    void _AddValueAndOperation(const std::string& value, StatementOperator op);

    //! Determines if the current statement buffer is true.
    //! \param scriptObject
    //! \return bool
    bool IsStatementBufferTrue(ScriptObject* scriptObject);

    //! Applies the entry to the current value.
    //! \param entry
    //! \param value
    void _Apply(StatementObjectEntry* entry, Variant& value);

    //! Clears out the entries.
    void _ClearEntries();

    //! Converts a symbol to a statement operator.
    //! \param symbol
    StatementOperator _SymbolToOp(const char* symbol);

public:
    //! Constructor.
    ScriptStatement();

    //! Called in place of the constructor.
    void __Construct() override;

    //! Builds the statement from a given string.
    //! \param varObject
    //! \param statementStr
    void Build(ScriptObject* varObject, StringBuffer* statementStr = nullptr);


    //! Adds a script object as part of the statement.
    //! \param scriptObject
    //! \param op
    StatementObjectEntry* Add(ScriptObject* scriptObject, StatementOperator op = StatementOperator::Equals);

    //! Adds a constant value as part of the statement.
    //! \param constantValue
    //! \param op
    StatementObjectEntry* Add(const Variant& constantValue, StatementOperator op = StatementOperator::Equals);

    //! Evaluates the statement.
    //! \return Variant
    Variant Evaluate();

    //! Returns evaluate value as a string.
    //! \return string
    std::string EvaluateAsString();

    //! Clones the statement based on the given script object.
    //! Note that this is for objects that were cloned from a
    //! similiar object so that the entries/values match.
    //! \param scriptObject
    //! \return ScriptStatement
    ScriptStatement* Clone(ScriptObject* scriptObject);

    //! Sets the string the statement should evaluate.
    //! \param StringBuffer
    inline void SetString(StringBuffer* buffer) {
        _String->Clear();
        _String->Append(buffer->CStr());
    }

    //! Returns the string.
    //! \return StringBuffer
    inline StringBuffer* GetString() const { return _String.Ptr(); }

    //! Sets the type for the statement.
    //! \param type
    inline void SetType(StatementType type) { _Type = type; }

    //! Returns the type for this statement.
    //! \return StatementType
    inline StatementType GetType() const { return _Type; }

    //! Returns the type as a string
    //! \return string
    std::string GetTypeAsString() const;

    //! Releases the object.
    bool Release() override;

    friend AllocManager;
};

sympl_nsend
