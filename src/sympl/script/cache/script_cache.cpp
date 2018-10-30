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
#include <sympl/script/cache/script_cache.h>
sympl_namespaces

ScriptCacheObject* ScriptCache::Fetch(ScriptObject* scriptObject)
{
    ScriptCacheObject* cacheObject = nullptr;
    const auto& objectAddress = scriptObject->GetObjectAddress();
    auto entry = _ObjectMap.find(objectAddress);

    if (entry == _ObjectMap.end()) {
        cacheObject = mem_alloc_ref(ScriptCacheObject);
        _ObjectMap[objectAddress] = cacheObject;

        cacheObject->SetId(objectAddress);
    } else {
        cacheObject = entry->second;
    }

    return cacheObject;
}

void ScriptCache::Remove(ScriptObject* scriptObject)
{
    const auto& objectAddress = scriptObject->GetObjectAddress();
    Remove(objectAddress);
}

void ScriptCache::Remove(MemAddressType objectAddress)
{
    auto entry = _ObjectMap.find(objectAddress);

    if (entry == _ObjectMap.end()) {
        return;
    }

    mem_free_ref(ScriptCacheObject, entry->second);
    _ObjectMap.erase(objectAddress);
}

void ScriptCache::Clear()
{
    for (const auto& entry : _ObjectMap) {
        mem_free_ref(ScriptCacheObject, entry.second);
    }
    _ObjectMap.clear();
}

