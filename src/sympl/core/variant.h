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
#include <sympl/core/ref.h>
#include <sympl/core/weak_ptr.h>

sympl_nsstart

class Object;
class StringBuffer;

/// Support types for the Variant.
enum class VariantType : uint8_t
{
    Empty = 0,
    Bool,
    Int,
    Float,

    StringBuffer,
    Object,

    MaxVariantTypes
};

/// Container for our Variant value.
struct VariantValue {
    union {
        bool                BoolVal;
        long long           IntVal;
        float               FloatVal;
        void*               Ptr;

    };

    VariantValue() { }
    VariantValue(const VariantValue& value) = delete;
    ~VariantValue() { }
};

class SYMPL_API Variant {
private:
    /// Type of variant.
    VariantType _Type = VariantType::Empty;
    /// Value for the variant object.
    VariantValue _Value;

public:
    //! Constructor.
    Variant()
    {
        _Value.Ptr = nullptr;
    }

    //! Constructor.
    //! \param value
    Variant(const Variant& value)
    {
        Set(value);
    }

    //! Constructor.
    //! \param value
    Variant(const bool value)
    {
        Set(value);
    }

    //! Constructor.
    //! \param value
    Variant(short value)
    {
        Set(value);
    }

    //! Constructor.
    //! \param value
    Variant(unsigned short value)
    {
        Set(value);
    }

    //! Constructor.
    //! \param value
    Variant(int value)
    {
        Set(value);
    }

    //! Constructor.
    //! \param value
    Variant(unsigned int value)
    {
        Set(value);
    }

    //! Constructor.
    //! \param value
    Variant(long value)
    {
        Set(value);
    }

    //! Constructor.
    //! \param value
    Variant(long long value)
    {
        Set(value);
    }

    //! Constructor.
    //! \param value
    Variant(unsigned long value)
    {
        Set(value);
    }

    //! Constructor.
    //! \param value
    Variant(float value)
    {
        Set(value);
    }

    //! Constructor.
    //! \param value
    Variant(double value)
    {
        Set(value);
    }

    //! Constructor.
    //! \param value
    Variant(const char* value)
    {
        Set(value);
    }

    //! Constructor.
    //! \param value
    Variant(StringBuffer* value)
    {
        Set(value);
    }

    //! Constructor.
    //! \param value
    Variant(Object* value);

    //! Destructor.
    ~Variant() {
        Clear();
    }

    //! Sets the type of the variant.
    //! \param type
    void SetType(VariantType type) {
        _Type = type;
    }

    //! Sets the value for the variant.
    //! \param value
    void Set(const Variant& value);

    //! Sets the value for the variant.
    //! \param value
    void Set(bool value) {
        Clear();

        SetType(VariantType::Bool);
        _Value.BoolVal = value;
    }

    //! Returns the variant value.
    //! \return bool
    bool GetBool() {
        if (_Type != VariantType::Bool) {
            return false;
        }
        return _Value.BoolVal;
    }

    //! Sets the value for the variant.
    //! \param value
    void Set(short value) {
        Clear();

        SetType(VariantType::Int);
        _Value.IntVal = static_cast<long long>(value);
    }

    //! Returns the variant value.
    //! \return short
    // short GetShort() {
    //     if (_Type != VariantType::UnsignedShort) {
    //         return 0;
    //     }
    //     return _Value.UnsignedShortVal;
    // }

    //! Sets the value for the variant.
    //! \param value
    void Set(unsigned short value) {
        Clear();

        SetType(VariantType::Int);
        _Value.IntVal = static_cast<long long>(value);
    }

    //! Returns the variant value.
    //! \return unsigned short
    // unsigned short GetUnsignedShort() {
    //     if (_Type != VariantType::UnsignedShort) {
    //         return 0;
    //     }
    //     return _Value.UnsignedShortVal;
    // }

    //! Sets the value for the variant.
    //! \param value
    void Set(int value) {
        Clear();

        SetType(VariantType::Int);
        _Value.IntVal = static_cast<long long>(value);
    }

    //! Sets the value for the variant.
    //! \param value
    void Set(unsigned int value) {
        Clear();

        SetType(VariantType::Int);
        _Value.IntVal = static_cast<long long>(value);
    }

    //! Returns the variant value.
    //! \return int
    long GetInt() {
        if (_Type != VariantType::Int) {
            return 0;
        }
        return _Value.IntVal;
    }

    //! Sets the value for the variant.
    //! \param value
    void Set(long value) {
        Clear();

        SetType(VariantType::Int);
        _Value.IntVal = static_cast<long long>(value);
    }

    //! Sets the value for the variant.
    //! \param value
    void Set(long long value) {
        Clear();

        SetType(VariantType::Int);
        _Value.IntVal = value;
    }

    //! Returns the variant value.
    //! \return long
    // int GetLong() {
    //     if (_Type != VariantType::Long) {
    //         return 0;
    //     }
    //     return _Value.LongVal;
    // }

    //! Sets the value for the variant.
    //! \param value
    void Set(unsigned long value) {
        Clear();

        SetType(VariantType::Int);
        _Value.IntVal = static_cast<long long>(value);
    }

    //! Returns the variant value.
    //! \return unsigned long
    // int GetUnsignedLong() {
    //     if (_Type != VariantType::UnsignedLong) {
    //         return 0;
    //     }
    //     return _Value.UnsignedLongVal;
    // }

    //! Sets the value for the variant.
    //! \param value
    void Set(float value) {
        Clear();

        SetType(VariantType::Float);
        _Value.FloatVal = value;
    }

    //! Returns the variant value.
    //! \return float
    float GetFloat() {
        if (_Type != VariantType::Float) {
            return 0;
        }
        return _Value.FloatVal;
    }

    //! Sets the value for the variant.
    //! \param value
    void Set(double value) {
        Clear();

        SetType(VariantType::Float);
        _Value.FloatVal = static_cast<float>(value);
    }

    //! Returns the variant value.
    //! \return double
    // double GetDouble() {
    //     if (_Type != VariantType::Double) {
    //         return 0;
    //     }
    //     return _Value.DoubleVal;
    // }

    //! Sets the value for the variant.
    //! \param value
    void Set(StringBuffer* value);

    //! Sets the value for the variant.
    //! \param value
    void Set(const char* value);

    //! Returns the string buffer.
    //! \return StringBuffer
    StringBuffer* GetStringBuffer();

    //! Sets the value for the variant.
    //! \param value
    void Set(Object* value);

    //! Returns the object.
    //! \return ObjectRef
    Object* GetObject();

    //! Returns the value as a string.
    //! \return string
    std::string AsString();

    //! Returns the type as a string.
    //! \return string
    std::string GetTypeAsString();

    //! Returns the variant type.
    //! \return VariantType
    inline VariantType GetType() const { return _Type; }

    //! Returns whether or not the variant is empty.
    //! \return bool
    inline bool IsEmpty() const { return _Type == VariantType::Empty; }

    //////////////////////////////////////////////////////////
    // Operator Overlading
    //////////////////////////////////////////////////////////

    //! Operator for assigning Variant.
    //! \param rhs
    //! \return
    Variant& operator =(const Variant& rhs) {
        Set(rhs);
        return *this;
    }

    //! Operator for assigning boolean.
    //! \param rhs
    //! \return
    Variant& operator =(bool rhs) {
        Set(rhs);
        return *this;
    }

    //! Operator for assigning short.
    //! \param rhs
    //! \return
    Variant& operator =(short rhs) {
        Set(rhs);
        return *this;
    }

    //! Operator for assigning unsigned short.
    //! \param rhs
    //! \return
    Variant& operator =(unsigned short rhs) {
        Set(rhs);
        return *this;
    }

    //! Operator for assigning integer.
    //! \param rhs
    //! \return
    Variant& operator =(int rhs) {
        Set(rhs);
        return *this;
    }

    //! Operator for assigning unsigned integer.
    //! \param rhs
    //! \return
    Variant& operator =(unsigned int rhs) {
        Set(rhs);
        return *this;
    }

    //! Operator for assigning long.
    //! \param rhs
    //! \return
    Variant& operator =(long rhs) {
        Set(rhs);
        return *this;
    }

    //! Operator for assigning long.
    //! \param rhs
    //! \return
    Variant& operator =(long long rhs) {
        Set(rhs);
        return *this;
    }

    //! Operator for assigning unsigned long.
    //! \param rhs
    //! \return
    Variant& operator =(unsigned long rhs) {
        Set(rhs);
        return *this;
    }

    //! Operator for assigning float.
    //! \param rhs
    //! \return
    Variant& operator =(float rhs) {
        Set(rhs);
        return *this;
    }

    //! Operator for assigning double.
    //! \param rhs
    //! \return
    Variant& operator =(double rhs) {
        Set(rhs);
        return *this;
    }

    //! Operator for assigning StringBuffer.
    //! \param rhs
    //! \return
    Variant& operator =(StringBuffer* rhs) {
        Set(rhs);
        return *this;
    }

    //! Operator for assigning StringBuffer.
    //! \param rhs
    //! \return
    Variant& operator =(const char* rhs) {
        Set(rhs);
        return *this;
    }

    //! Operator for assigning StringBuffer.
    //! \param rhs
    //! \return
    Variant& operator =(const std::string& rhs) {
        Set(rhs.c_str());
        return *this;
    }

    //! Operator for assigning ObjectRef.
    //! \param rhs
    //! \return
    Variant& operator =(Object* rhs);

    //! Operator for assigning ObjectRef.
    //! \param rhs
    //! \return
    const bool operator ==(const Variant& rhs) {
        return (
            _Type == rhs.GetType() &&
            _Value.Ptr == rhs._Value.Ptr
        );
    }

    //! Clears out the variant.
    void Clear();

    /// Empty script object.
    static Variant Empty;
};

sympl_nsend
