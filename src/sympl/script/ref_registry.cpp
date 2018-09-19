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
#include <sympl/script/ref_registry.h>
#include <sympl/script/script_object.h>
#include <sympl/core/string_buffer.h>
#include <sympl/util/string_helper.h>
#include <fmt/format.h>
sympl_namespaces

std::string RefRegistry::Register(ScriptObject* entry)
{
    sympl_assert(_AddressMap.size() < SYMPL_MAX_REFERENCES && "Too many referenced objects created!");

    auto address = StringHelper::GenerateRandomStr(64);
    entry->SetMeta("RefAddress", address.c_str());

    _AddressMap[address] = entry;

    return address;
}

ScriptObject* RefRegistry::FindObject(std::string address)
{
    auto entry = _AddressMap.find(address);
    if (entry == _AddressMap.end()) {
        return &ScriptObject::Empty;
    }
    return entry->second;
}

void RefRegistry::Remove(ScriptObject* entry)
{
    if (!entry->HasMeta("RefAddress")) {
        return;
    }

    auto address = entry->GetMeta("RefAddress").GetStringBuffer()->CStr();
    _AddressMap.erase(address);
}
