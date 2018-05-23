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
#include <sympl/script/script_context.h>
#include <sympl/script/script_object.h>
sympl_namespaces

ScriptContext::ScriptContext()
{
    _Owner = &ScriptObject::Empty;
    _CurrentScope = &ScriptObject::Empty;
    _Caller = &ScriptObject::Empty;
}

ScriptContext::~ScriptContext()
{

}

void ScriptContext::SetOwner(ScriptObject* owner)
{
    _Owner = (!IsNullObject(owner) && !owner->IsEmpty() ? owner : &ScriptObject::Empty);
}

ScriptObject* ScriptContext::GetOwner() const
{
    return (_Owner.IsValid() ? _Owner.Ptr() : &ScriptObject::Empty);
}

void ScriptContext::SetCurrentScope(ScriptObject* scopeObject)
{
    _CurrentScope = (!IsNullObject(scopeObject) && !scopeObject->IsEmpty() ? scopeObject : &ScriptObject::Empty);
}

ScriptObject* ScriptContext::GetCurrentScope() const
{
    return (_CurrentScope.IsValid() ? _CurrentScope.Ptr() : &ScriptObject::Empty);
}

void ScriptContext::SetCaller(ScriptObject* caller)
{
    _Caller = (!IsNullObject(caller) && !caller->IsEmpty() ? caller : &ScriptObject::Empty);
}

ScriptObject* ScriptContext::GetCaller() const
{
    return (_Caller.IsValid() ? _Caller.Ptr() : &ScriptObject::Empty);
}
