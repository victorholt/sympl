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

#include <sympl/core/sympl_object.h>
#include <sympl/core/shared_ref.h>
#include <sympl/core/weak_ref.h>

sympl_nsstart

class ScriptObject;

struct ScriptContextCallEntry
{
    /// Owner of the context.
    SharedRef<ScriptObject> Owner;

    /// Reference to the current scope of the context.
    SharedRef<ScriptObject> CurrentScope;

    /// Reference to the caller.
    SharedRef<ScriptObject> Caller;
};

class SYMPL_API ScriptContext : public Object
{
    SYMPL_OBJECT(ScriptContext, Object);

protected:
    /// Call list of entries in the scope.
    std::vector<ScriptContextCallEntry> _Entries;

    /// Current return value.
    Variant _RetValue;

    /// Flag to signal an exit.
    bool _Exit = false;

public:
    //! Constructor.
    ScriptContext();

    //! Destructor.
    ~ScriptContext() override;

    //! Adds a new entry to the list.
    //! \param entry
    void AddEntry(const ScriptContextCallEntry& entry);

    //! Returns all entries in the list.
    //! \return std::vector<ScriptContextCallEntry>
    const std::vector<ScriptContextCallEntry>& GetEntries() const;

    //! Sets the return value.
    //! \param value
    inline void SetReturnValue(const Variant& value) { _RetValue = value; }

    //! Returns the return value.
    //! \return Variant
    inline Variant GetReturnValue() const { return _RetValue; }

    //! Sets the exit flag.
    //! \param exit
    inline void SetExit(bool exit) { _Exit = exit; }

    //! Returns the exit flag.
    //! \return bool
    inline bool GetExit() const { return _Exit; }
};

sympl_nsend
