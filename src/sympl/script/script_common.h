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
#include <sympl/core/registry.h>
#include <sympl/core/object.h>
#include <sympl/core/variant.h>
#include <sympl/core/weak_ptr.h>
#include <sympl/util/profiler.h>

sympl_nsstart

class ScriptObject;

#define GLOBAL_SCRIPT_OBJECT "__global__"
#define SYMPL_CLASS_TOKEN "class@@"
#define SYMPL_STRING_TOKEN "%dq%"
#define SYMPL_SCOPE_NAME "__scope__"
#define SYMPL_METHOD_ARG_NAME "__arg__"
#define to_script_object(var) dynamic_cast<ScriptObject*>(var.GetObject())

#define ScriptMethodArgs Urho3D::PODVector<Variant>&
#define ScriptMethodArgList Urho3D::PODVector<Variant>
typedef std::function<void(ScriptMethodArgs)> ScriptMethodCallback;

enum class ScriptObjectType : uint8_t {
    Empty = 0,
    Object,
    Reference,
    Variable,
    Array,
    Method,
};

enum class StatementOperator {
    None = 0,
    Equals,
    Add,
    Subtract,
    Divide,
    Multiply,
    Mod,
    IsEqual2,
    NotIsEqual2,
    GreaterThan,
    LessThan,
    GreaterEqualThan,
    LessEqualThan,
    NewObject,
    DeleteObject,
};

enum class StatementType {
    None = 0,
    Object,
    String,
    Bool,
    Integer,
    Float,
};

struct StatementEntry
{
    SharedPtr<ScriptObject> ObjectValue;
    Variant ConstantValue;
    StatementOperator Op;
};

struct StatementCacheEntry
{
    char Statement[256];
    std::vector<StatementEntry*> Entries;
};

sympl_nsend


