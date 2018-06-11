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

class StatementResolver;
class SYMPL_API EvalResolver : public Object
{
    SYMPL_OBJECT(EvalResolver, Object);

public:
    //! Constructor.
    EvalResolver() = default;

    //! Called in place of the constructor.
    void __Construct() override {}

    //! Returns a value from evaluating a given script object.
    //! \param stmtStr
    //! \param scriptObject
    //! \return
    Variant GetEvalFromStatementBuffer(const char* stmtStr, ScriptObject* scriptObject);

    //! Returns whether or not a given script object evaluates to true.
    //! \param stmtStr
    //! \param scriptObject
    //! \return
    bool IsStatementBufferTrue(const char* stmtStr, ScriptObject* scriptObject);
};

class SYMPL_API MethodResolver : public Object
{
    SYMPL_OBJECT(MethodResolver, Object);

public:
    MethodResolver() = default;

    //! Called in place of the constructor.
    void __Construct() override {}

    //! Resolves the method.
    //! \param stmtResolver
    //! \param currentStr
    //! \param varObject
    //! \param op
    //! \return
    virtual Variant Resolve(StatementResolver* stmtResolver, StringBuffer* currentStr,
                            ScriptObject* varObject, StatementOperator op);
};

class SYMPL_API ParenthResolver : public Object
{
    SYMPL_OBJECT(ParenthResolver, Object);

public:
    ParenthResolver() = default;

    //! Called in place of the constructor.
    void __Construct() override {}

    //! Resolves the parentheses.
    //! \param stmtResolver
    //! \param currentStr
    //! \param varObject
    //! \param op
    //! \return
    virtual Variant Resolve(StatementResolver* stmtResolver, StringBuffer* currentStr,
                            ScriptObject* varObject, StatementOperator op);
};

class SYMPL_API StatementResolver : public Object
{
    SYMPL_OBJECT(StatementResolver, Object);

private:
    /// Statement string we want to resolve.
    StringBuffer* _StmtString = nullptr;

    /// Reference to the script token helper.
    ScriptToken* _TokenHelper = nullptr;

    /// Current character location.
    size_t _CurrentCharLocation = 0;

    /// Type of statement.
    StatementType _Type = StatementType::None;

    // Statement entries.
    std::vector<StatementEntry*> _StmtEntries;

    //! Attempts to resolve a given statement.
    //! \param stmtEntries
    //! \return Variant
    Variant _ResolveStatements(const std::vector<StatementEntry*>& stmtEntries);

    //! Solves a statement entry with a current value.
    //! \param entry
    //! \param value
    void _Solve(StatementEntry* entry, Variant& value);

    //! Checks if a given string is a number.
    //! \param str
    //! \return Variant
    Variant _IsNumber(const char* str);

    //! Checks if a given string is a boolean value.
    //! \param str
    //! \return Variant
    Variant _IsBoolean(const char* str);

    //! Finds the statement type.
    //! \param value
    //! \return StatementType
    StatementType _FindType(const Variant& value);

    //! Converts a symbol to a statement operator.
    //! \param symbol
    StatementOperator _SymbolToOp(const char* symbol);

public:
    //! Constructor.
    StatementResolver();

    //! Called in place of the constructor.
    void __Construct() override;

    //! Attempts to resolve a given string.
    //! \param str
    //! \param varObject
    //! \param cache
    Variant Resolve(const char* str, ScriptObject* varObject, bool cache = false);

    //! Clears the statement entries.
    void ClearStatementEntries();

    //! Release the object.
    //! \return bool
    bool Release() override;

    //! Returns the statement string.
    //! \return StringBuffer
    inline StringBuffer* GetStatementString() const { return _StmtString; }

    //! Sets the current character location.
    //! \param location
    inline void SetCurrentCharLocation(size_t location) { _CurrentCharLocation = location; }

    //! Returns the current character location of the statement.
    //! \return size_t
    inline size_t GetCurrentCharLocation() const { return _CurrentCharLocation; }

    //! Sets the statement type.
    //! \param type
    inline void SetType(StatementType type) { _Type = type; }

    //! Returns the statement type.
    //! \return StatementType
    inline StatementType GetType() const { return _Type; }

    friend EvalResolver;
    friend MethodResolver;
    friend ParenthResolver;
};

sympl_nsend
