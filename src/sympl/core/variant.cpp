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
#include <sympl/core/string_buffer.h>
#include <sympl/core/object.h>
#include <sympl/script/script_object.h>

#include <fmt/format.h>
sympl_namespaces

Variant Variant::Empty;

Variant::Variant(Object* value)
        : _Type(VariantType::Empty)
{
    Set(value);
}

void Variant::Set(Object* value) {
    Clear();
    value->AddRef();

    SetType(VariantType::Object);
    _Value.Ptr = value;
}

Object* Variant::GetObject() {
    if (_Type != VariantType::Object) {
        return nullptr;
    }
    return reinterpret_cast<Object*>(_Value.Ptr);
}

Variant& Variant::operator =(Object* rhs) {
    Set(rhs);
    return *this;
}

void Variant::Set(const Variant& value)
{
    Clear();

    _Type = value._Type;

    switch ((int)_Type) {
        case (int)VariantType::Bool:
            _Value.BoolVal = value._Value.BoolVal;
            break;
        case (int)VariantType::Int:
            _Value.IntVal = value._Value.IntVal;
            break;
        case (int)VariantType::Float:
            _Value.FloatVal = value._Value.FloatVal;
            break;
        case (int)VariantType::Object:
        case (int)VariantType::StringBuffer:
            _Value.Ptr = value._Value.Ptr;
            break;
    }

    // Increment the reference count so we don't run into
    // a segment fault situation when passing the pointer
    // around.
    if (_Type == VariantType::StringBuffer) {
        auto obj = GetStringBuffer();
        if (obj->GetManagedRef()) {
            GetStringBuffer()->AddRef();
        }
    }
    if (_Type == VariantType::Object) {
        auto obj = GetObject();
        if (obj->GetManagedRef()) {
            GetObject()->AddRef();
        }
    }
}

void Variant::Set(StringBuffer* value) {
    Clear();

    if (value->GetManagedRef()) {
        value->AddRef();
    }

    SetType(VariantType::StringBuffer);
    _Value.Ptr = value;
}

void Variant::Set(const char* value) {
    // Only clear if we are not already a string buffer.
    if (_Type != VariantType::StringBuffer) {
        Clear();

        if (IsNullObject(_Value.Ptr) || _Type == VariantType::Empty) {
            auto sb = mem_alloc_ref(StringBuffer);
            Set(sb);
        }
    }

    SetType(VariantType::StringBuffer);
    GetStringBuffer()->Clear();
    GetStringBuffer()->Append(value);
}

StringBuffer* Variant::GetStringBuffer() {
    if (_Type != VariantType::StringBuffer) {
        return nullptr;
    }
    return reinterpret_cast<StringBuffer*>(_Value.Ptr);
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
        case (int)VariantType::Object:
            value = "[object]";
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
        case (int)VariantType::Object:
            return "object";
    }

    return "unknown";
}

void Variant::Clear() {
    // Free our string buffer.
    if (_Type == VariantType::StringBuffer) {
        auto buffer = GetStringBuffer();
        if (buffer->GetManagedRef()) {
            mem_free_ref(StringBuffer, buffer);
        }
        _Value.Ptr = nullptr;
    }

    // Free our object.
    if (_Type == VariantType::Object) {
        auto sobj = GetObject();

        if (sobj->GetTypeName() != "") {
            mem_free_ref(Object, sobj);
        }

        _Value.Ptr = nullptr;
    }

    _Type = VariantType::Empty;
}
