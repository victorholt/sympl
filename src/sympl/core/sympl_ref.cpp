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
#include <sympl/core/sympl_ref.h>
sympl_namespaces

RefInfo::RefInfo(const char* typeName, const RefInfo* baseTypeInfo)
    :   _TypeName(typeName),
        _BaseTypeInfo(baseTypeInfo)
{

}

RefInfo::~RefInfo() = default;

bool RefInfo::IsTypeOf(std::string type) const
{
    const RefInfo* current = this;

    while (current)
    {
        if (current->GetTypeName() == type)
            return true;

        current = current->GetBaseTypeInfo();
    }

    return false;
}

bool RefInfo::IsTypeOf(const RefInfo* typeInfo) const
{
    const RefInfo* current = this;

    while (current)
    {
        if (current == typeInfo)
            return true;

        current = current->GetBaseTypeInfo();
    }

    return false;
}

bool Ref::Free()
{
    if (_RefCount > 0) {
        _RefCount--;
    }
    return (_RefCount <= 0);
}
