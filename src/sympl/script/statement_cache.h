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

#include <sympl/core/variant.h>
#include <sympl/core/object.h>

#include <sympl/script/script_common.h>

sympl_nsstart

class SYMPL_API StatementCache : public Object
{
    SYMPL_OBJECT(StatementCache, Object);

protected:
    /// Quick variable access list.
    std::vector<StatementCacheEntry*> _CacheEntries;

public:
    //! Constructor.
    StatementCache();

    //! Called in place of the constructor.
    void __Construct() override;

    //! Cache a statement entry.
    //! \param cache
    //! \param entry
    StatementCacheEntry* Cache(const char* cache, const std::vector<StatementEntry*>& entries);

    //! Attempts to return a cache entry.
    //! \param cache
    //! \return StatementCacheEntry
    StatementCacheEntry* GetEntry(const char* cache);

    //! Release the object.
    //! \return bool
    bool Release() override;
};

sympl_nsend
