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
#include "variant.h"
#include "../script/script_object.h"
#include "../script/script_statement.h"
sympl_namespaces

Variant Variant::Empty;

Variant::Variant(ScriptObject* value) {
    Set(value);
}

void Variant::Set(ScriptObject* value) {
    SetType(VariantType::ScriptObject);
    _Value.Ptr = value;
}

ScriptObject* Variant::GetScriptObject() {
    if (_Type != VariantType::ScriptObject) {
        return nullptr;
    }
    return reinterpret_cast<ScriptObject*>(_Value.Ptr);
}

Variant& Variant::operator =(ScriptObject* rhs) {
    Set(rhs);
    return *this;
}

Variant::Variant(ScriptStatement* value) {
    Set(value);
}

void Variant::Set(ScriptStatement* value) {
    SetType(VariantType::ScriptStatement);
    _Value.Ptr = value;
}

ScriptStatement* Variant::GetScriptStatement() {
    if (_Type != VariantType::ScriptStatement) {
        return nullptr;
    }
    return reinterpret_cast<ScriptStatement*>(_Value.Ptr);
}

Variant& Variant::operator =(ScriptStatement* rhs) {
    Set(rhs);
    return *this;
}

void Variant::Free() {
    // Check to see if we have a reference.
    if (IsNullObject(_Value.Ptr)) {
        return;
    }

    // Free our string buffer.
    if (_Type == VariantType::StringBuffer) {
        StringBuffer* buffer = GetStringBuffer();
        free_ref(StringBuffer, buffer);
        _Value.Ptr = nullptr;
    }

    // Free our script object.
    if (_Type == VariantType::ScriptObject) {
        ScriptObject* sobj = GetScriptObject();
        free_ref(ScriptObject, sobj);
        _Value.Ptr = nullptr;
    }

    // Free our script statement.
    if (_Type == VariantType::ScriptObject) {
        ScriptStatement* sobj = GetScriptStatement();
        free_ref(ScriptStatement, sobj);
        _Value.Ptr = nullptr;
    }
}