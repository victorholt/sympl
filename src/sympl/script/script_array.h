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
#include <sympl/core/shared_ptr.h>
#include <sympl/script/script_object.h>

sympl_nsstart

class SYMPL_API ScriptArray : public ScriptObject
{
    SYMPL_OBJECT(ScriptArray, ScriptObject);

protected:
    /// Items in the array.
    std::unordered_map<std::string, Variant> _Items;

public:
    //! Constructor.
    ScriptArray();

    //! Sets an item to the array.
    //! \param index
    //! \param item
    void SetItem(const char* index, const Variant& item);

    //! Removes an item from the array.
    //! \param index
    void RemoveItem(size_t index);

    //! Removes an item from the array.
    //! \param index
    void RemoveItem(const char* index);

    //! Returns a string representation of the array.
    std::string GetArrayString();

    //! Returns an item from the array.
    //! \param index
    //! \return Variant
    const Variant& GetItem(size_t index);

    //! Returns an item from the array.
    //! \param index
    //! \return Variant
    const Variant& GetItem(const std::string& index);

    //! Called in place of the constructor.
    void __Construct() override;

    //! Releases the object.
    bool Release() override;

    //! Returns items in the array.
    //! \return std::vector<Variant>
    inline const std::unordered_map<std::string, Variant>& GetItems() const { return _Items; }

    //! Returns the length of the array.
    //! \return size_t
    inline size_t Length() const { return _Items.size(); }
};

#define to_array(obj) dynamic_cast<ScriptArray*>(obj)

sympl_nsend
