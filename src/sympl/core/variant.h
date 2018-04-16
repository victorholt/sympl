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

#include "sympl_pch.h"

sympl_nsstart

enum class VariantType : uint8_t
{
    Bool = 0,
    Byte,
    Char,
    Short,
    UnsignedShort,
    Int,
    UnsignedInt,
    Long,
    UnsignedLong,
    Float,
    Double
};

template<class T>
class SYMPL_API Variant {
private:
    typedef union {
        bool            _bVal;
        unsigned char   _byteVal;
        char            _charVal;
        short           _sVal;
        unsigned short  _usVal;
        int             _iVal;
        unsigned int    _uiVal;
        long            _lVal;
        unsigned long   _ulVal;
        float           _fVal;
        double          _dVal;
    } DataType;

    DataType _Data;

public:
    inline void Set(const T val) {
        _Data = val;
    }
    const T Get() const {
        return reinterpret_cast<T>(_Data);
    }
};

sympl_nsend