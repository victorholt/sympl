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
#include <sympl/core/alloc_manager.h>
#include <sympl/core/ref.h>
#include <sympl/core/variant.h>

#include <sympl/thirdparty/urho3d/container/Vector.h>

sympl_nsstart

class SYMPL_API Object : public RefCounter {
protected:
    /// Meta keys for the object.
    Urho3D::PODVector<char*> _MetaKeys;

    /// Meta values for the object.
    Urho3D::PODVector<Variant> _MetaValues;

public:
    //! Destructor.
    ~Object();

    //! Sets meta data for the object.
    //! \param key
    //! \param value
    void SetMeta(const char* key, const Variant& value);

    //! Returns meta data for the object.
    //! \return Variant
    Variant GetMeta(const char* key);

    //! Returns whether or not the meta key exists.
    //! \return bool
    bool HasMeta(const char* key);
};

sympl_nsend
