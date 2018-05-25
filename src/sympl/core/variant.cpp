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
#include <sympl/core/variant.h>
#include <sympl/core/sympl_object.h>
#include <sympl/core/string_buffer.h>
#include <sympl/script/script_object.h>
#include <sympl/script/script_statement.h>

#include <fmt/format.h>
sympl_namespaces

Variant Variant::Empty;

Variant::Variant(ScriptObject* value) {
    Set(value);
}

void Variant::Set(ScriptObject* value) {
    Clear();
    value->AddRef();

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
    Clear();
    value->AddRef();

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

void Variant::Set(const Variant& value)
 {
     Clear();

    _Type = value._Type;
    _Value = value._Value;

    // Increment the reference count so we don't run into
    // a segment fault situation when passing the pointer
    // around.
    if (_Type == VariantType::StringBuffer) {
        GetStringBuffer()->AddRef();
    }
    if (_Type == VariantType::ScriptObject) {
        GetScriptObject()->AddRef();
    }
    if (_Type == VariantType::ScriptStatement) {
        GetScriptStatement()->AddRef();
    }
}

void Variant::Set(StringBuffer* value) {
    Clear();

    value->AddRef();

    SetType(VariantType::StringBuffer);
    _Value.Ptr = value;
}

void Variant::Set(const char* value) {
    // Only clear if we are not already a string buffer.
    if (_Type != VariantType::StringBuffer) {
        Clear();

        if (IsNullObject(_Value.Ptr) || _Type == VariantType::Empty) {
            Set(alloc_ref(StringBuffer));
        }
    }

    GetStringBuffer()->Clear();
    GetStringBuffer()->Append(value);
}

StringBuffer* Variant::GetStringBuffer() {
    if (_Type != VariantType::StringBuffer) {
        return nullptr;
    }
    return reinterpret_cast<StringBuffer*>(_Value.Ptr);
}

void Variant::Clear() {
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
    if (_Type == VariantType::ScriptStatement) {
        ScriptStatement* sobj = GetScriptStatement();
        free_ref(ScriptStatement, sobj);
        _Value.Ptr = nullptr;
    }
}

std::string Variant::AsString()
{
    std::string value;

    switch ((int)_Type) {
        case (int)VariantType::Empty:
            break;
        case (int)VariantType::Bool:
            value = (GetBool() ? "true" : "false");
            break;
        case (int)VariantType::Int:
            value = fmt::format("{0}", GetInt());
            break;
        case (int)VariantType::Float:
            value = fmt::format("{0:f}", GetFloat());
            break;
        case (int)VariantType::StringBuffer:
            value = GetStringBuffer()->CStr();
            break;
        case (int)VariantType::ScriptObject:
            value = "[object]";
            break;
        case (int)VariantType::ScriptStatement:
            value = "[statement]";
            break;
    }

    return value;
}

std::string Variant::GetTypeAsString()
{
    switch ((int)_Type) {
        case (int)VariantType::Empty:
            return "empty";
        case (int)VariantType::Bool:
            return "bool";
        case (int)VariantType::Int:
            return "int";
        case (int)VariantType::Float:
            return "float";
        case (int)VariantType::StringBuffer:
            return "string";
        case (int)VariantType::ScriptObject:
            return "object";
        case (int)VariantType::ScriptStatement:
            return "statement";
    }

    return "unknown";
}
