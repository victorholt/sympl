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
#include <sympl/script/script_object.h>
#include <sympl/script/cache/script_cache_object.h>

sympl_nsstart

class SYMPL_API ScriptCache
{
protected:
    /// Quick variable access list.
    std::unordered_map<std::string, ScriptCacheObject*> _ObjectMap;

    //! Constructor.
    ScriptCache() = default;

public:

    //! Destructor.
    ~ScriptCache() {
        Clear();
    }

    //! Returns the singleton for the class.
    //! \return StatementCache
    static ScriptCache& GetInstance() {
        static ScriptCache instance;
        return instance;
    }

    //! Attempts to return a cache entry.
    //! \param scriptObject
    //! \return ScriptCacheObject
    ScriptCacheObject* Fetch(ScriptObject* scriptObject);

    //! Remove all cache for an object address.
    //! \param scriptObject
    void Remove(ScriptObject* scriptObject);

    //! Removes all cache for an object address.
    //! \param objectAddress
    void Remove(const std::string& objectAddress);

    //! Clears the cache.
    void Clear();
};

#define ScriptCacheInstance ScriptCache::GetInstance()

sympl_nsend
