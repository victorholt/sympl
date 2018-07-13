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
#include <sympl/script/statement_cache.h>
sympl_namespaces

StatementCache::StatementCache()
{

}

void StatementCache::__Construct()
{

}

StatementCacheEntry* StatementCache::Cache(const char* cache, const std::vector<StatementEntry*>& entries)
{
    if (strlen(cache) == 0 || *cache == '\0' || *cache == ' ') {
        return nullptr;
    }

    StatementCacheEntry* entry = GetEntry(cache);
    if (!IsNullObject(entry)) {
        return entry;
    }

    entry = mem_alloc_object(StatementCacheEntry);
    memset(entry->Statement, 0, strlen(entry->Statement));
    memcpy(entry->Statement, cache, strlen(cache));

    auto num = entries.size();
    for (int i = 0; i < num; i++) {
        StatementEntry* stmtEntry = mem_alloc_object(StatementEntry);
        stmtEntry->ObjectValue = entries[i]->ObjectValue;
        stmtEntry->ConstantValue = entries[i]->ConstantValue;
        stmtEntry->Op = entries[i]->Op;
        entry->Entries.push_back(stmtEntry);
    }

    _CacheEntries.push_back(entry);

    return entry;
}

StatementCacheEntry* StatementCache::GetEntry(const char* cache)
{
    auto num = _CacheEntries.size();
    for (int i = 0; i < num; i++) {
        if (strcmp(_CacheEntries[i]->Statement, cache) == 0) {
            return _CacheEntries[i];
        }
    }
    return nullptr;
}

bool StatementCache::Release()
{
    auto count = _CacheEntries.size();
    for (int i = 0; i < count; i++) {
        auto ecount = _CacheEntries[i]->Entries.size();
        for (int j = 0; j < ecount; j++) {
            mem_free_object(StatementEntry, _CacheEntries[i]->Entries[j]);
        }
        mem_free_object(StatementCacheEntry, _CacheEntries[i]);
    }
    _CacheEntries.clear();
    return true;
}