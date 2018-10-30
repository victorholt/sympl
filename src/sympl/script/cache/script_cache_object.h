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

sympl_nsstart

class SYMPL_API ScriptCacheObject : public Object
{
    SYMPL_OBJECT(ScriptCacheObject, Object);

protected:
    /// Map (i.e. Statement string/entries) associated with the object.
    std::unordered_map< std::string, std::vector< Object* > > _ValueMap;

    /// Cache id (ScriptObject Address).
    MemAddressType _Id;

public:
    //! Called in place of the constructor.
    void __Construct() override {};

    //! Fetches the value based on the given key.
    //! \param key
    //! \return bool
    template<typename T>
    bool Fetch(const std::string& key, std::vector<T>& output)
    {
        auto it = _ValueMap.find(key);

        if (it == _ValueMap.end()) {
            return false;
        }

        for (auto& entry : it->second) {
            output.emplace_back(dynamic_cast<T>(entry));
        }

        return true;
    }

    //! Returns the values based on the given cache key.
    //! \param key
    //! \return bool
    bool FetchValues(const std::string& key, std::vector<Object*>*& output);

    //! Stores a value for the object.
    //! \param key
    //! \param value
    void Store(const std::string& key, Object* value);

    //! Checks whether or not a key exists.
    //! \param key
    //! \return bool
    bool HasKey(const std::string& key);

    //! Removes a key from the object.
    //! \param key
    void RemoveKey(const std::string& key);

    //! Clears all values.
    void Clear();

    //! Sets the id.
    //! \param value
    inline void SetId(MemAddressType value) { _Id = value; }

    //! Returns the id.
    //! \return string
    inline MemAddressType GetId() const { return _Id; }
};

sympl_nsend
