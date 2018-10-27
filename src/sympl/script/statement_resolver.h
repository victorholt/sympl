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
#include <sympl/core/string_buffer.h>
#include <sympl/core/object.h>
#include <sympl/core/shared_ptr.h>
#include <sympl/script/script_object.h>
#include <sympl/script/script_token.h>
#include <sympl/script/statement_entry.h>

sympl_nsstart

class ScriptCacheObject;

class SYMPL_API StatementResolver : public Object
{
    SYMPL_OBJECT(StatementResolver, Object);

protected:
    /// String for the statement.
    SharedPtr<StringBuffer> _StmtString;

    /// Entries for the statement.
    std::vector<StatementEntry*> _StmtEntries;

    /// Reference to the token helper.
    ScriptToken* _TokenHelper;

    /// Current character location.
    size_t _CharLocation = 0;

    /// Type of statement.
    StatementType _Type = StatementType::None;

    /// Flag to cache the statement entries.
    bool _Cache = false;

    /// Reference to the cache object.
    ScriptCacheObject* _CacheObject = nullptr;

    /// List of the cached entries.
    std::vector<StatementEntry*> _CacheEntries;

    /// String of the cache key.
    std::string _CacheKey;

    //! Attempts to resolve a given statement.
    //! \param stmtEntries
    //! \return Variant
    Variant _ResolveStatements(const std::vector<StatementEntry*>& stmtEntries);

    //! Solves a statement entry with a current value.
    //! \param entry
    //! \param value
    void _Solve(StatementEntry* entry, Variant& value);

    //! Converts a symbol to a statement operator.
    //! \param symbol
    StatementOperator _SymbolToOp(const char* symbol);

    //! Checks if a given string is a boolean value.
    //! \param str
    //! \return Variant
    Variant _IsBoolean(const char* str);

    //! Finds the statement type.
    //! \param value
    //! \return StatementType
    StatementType _FindType(const Variant& value);

public:
    //! Constructor.
    StatementResolver();

    //! Called in place of the constructor.
    void __Construct() override;

    //! Resolves a given statement into the destination object.
    //! \param stmtStr
    //! \param destObject
    Variant Resolve(const std::string& stmtStr, ScriptObject* destObject);

    //! Clears the statement entries.
    void ClearStatementEntries();

    //! Sets the cache flag for the statement.
    //! \param value
    void SetCache(bool value);

    //! Releases the object.
    bool Release() override;

    //! Sets the current char location.
    //! \param value
    inline void SetCharLocation(size_t value) { _CharLocation = value; }

    //! Returns the statement string buffer.
    //! \return StringBuffer
    inline StringBuffer* GetStatementString() const { return _StmtString.Ptr(); }

    //! Returns the current character location.
    //! \return size_t
    inline size_t GetCharLocation() const { return _CharLocation; }
};

sympl_nsend
