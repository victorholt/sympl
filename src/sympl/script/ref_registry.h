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
#include <sympl/core/variant.h>
#include <sympl/core/shared_ptr.h>

sympl_nsstart

class ScriptObject;

#define SYMPL_MAX_REFERENCES 100000

class SYMPL_API RefRegistry
{
protected:
    /// Address map for the entries.
    std::unordered_map<std::string, ScriptObject*> _AddressMap;

    //! Constructor.
    RefRegistry() {
        _AddressMap.reserve(SYMPL_MAX_REFERENCES);
    }

public:
    //! Destructor.
    ~RefRegistry() {
        Clear();
    }

    //! Returns the singleton for our class.
    //! \return RefRegistry
    static RefRegistry& GetInstance() {
        static RefRegistry instance;
        return instance;
    }

    //! Registers an object and returns the address (used for scriptobject paths).
    //! \param entry
    //! \return
    std::string Register(ScriptObject* entry);

    //! Attempts to find an address.
    //! \param address
    //! \return ScriptObject
    ScriptObject* FindObject(std::string address);

    //! Removes a referenced object.
    //! \param address
    bool RemoveAddress(std::string address);

    //! Clears the address map.
    void Clear();
};

#define SymplRefRegistry RefRegistry::GetInstance()

sympl_nsend
