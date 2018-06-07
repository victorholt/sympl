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
#include <sympl/core/object.h>
sympl_namespaces

Object::~Object()
{
    for (auto key : _MetaKeys) {
//        delete [] key;
        free_bytes_array(key);
    }
    _MetaKeys.Clear();
    _MetaValues.Clear();
}

void Object::SetMeta(const char* key, const Variant& value)
{
//    auto data = new char[strlen(key) + 1];//alloc_bytes_array(char, strlen(key) + 1);
    auto data = alloc_bytes_array(char, strlen(key) + 1);
    memset(data, 0, strlen(key) + 1);
    memcpy(data, key, strlen(key));

    _MetaKeys.Push(data);
    _MetaValues.Push(value);
}

Variant Object::GetMeta(const char* key)
{
    for (size_t i = 0; i < _MetaKeys.Size(); i++) {
        if (strcmp(_MetaKeys[i], key) == 0) {
            return _MetaValues[i];
        }
    }
    return Variant::Empty;
}

bool Object::HasMeta(const char* key)
{
    auto meta = GetMeta(key);
    return (meta.GetType() != VariantType::Empty);
}
