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
#include <sympl/script/script_array.h>
sympl_namespaces

ScriptArray::ScriptArray()
{

}

void ScriptArray::__Construct()
{

}

void ScriptArray::SetItem(const char* index, const Variant& item)
{
    _Items[index] = item;
}

void ScriptArray::RemoveItem(size_t index)
{
    RemoveItem(std::to_string(index).c_str());
}

void ScriptArray::RemoveItem(const char* index)
{
    auto item = _Items.find(index);
    if (item == _Items.end()) {
        sympl_assert(false, "Index outside of array bounds in removal attempt!");
        return;
    }
    _Items.erase(index);
}

std::string ScriptArray::GetArrayString()
{
    std::string value = "[";
    auto numItems = _Items.size();
//    for (size_t i = 0; i < numItems; i++) {
//        if (i > 0) {
//            value.append(",");
//        }
//
//        auto entry = _Items[i];
//        value.append(entry.AsString());
//    }
    value.append("]");
    return value;
}

const Variant& ScriptArray::GetItem(size_t index)
{
    return GetItem(std::to_string(index).c_str());
}

const Variant& ScriptArray::GetItem(const std::string& index)
{
    auto item = _Items.find(index);
    if (item == _Items.end()) {
        sympl_assert(false, "Index outside of array bounds!");
        return Variant::Empty;
    }
    return _Items[index];
}

bool ScriptArray::Release()
{
    _Items.clear();
    return true;
}
