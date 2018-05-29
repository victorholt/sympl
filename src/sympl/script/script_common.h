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
#include <sympl/core/object_ref.h>
#include <sympl/core/variant.h>
#include <sympl/core/weak_ptr.h>
#include <sympl/script/script_common.h>

sympl_nsstart

#define SYMPL_STRING_TOKEN "@__STRING__@"
#define SYMPL_SCOPE_NAME "__scope__"

class ScriptObject;
typedef std::function<void(const std::vector<WeakPtr<ScriptObject>>&)> SymplMethodCallback;

enum class ScriptObjectType : uint8_t {
    Empty = 0,
    Object,
    Variable,
    Array,
    Method,
    Statement
};

sympl_nsend


