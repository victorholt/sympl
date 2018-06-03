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
#include <sympl/script/method_registry.h>
#include <sympl/script/script_vm.h>

#include <sympl/util/profiler.h>
sympl_namespaces

MethodRegistry::MethodRegistry()
{

}

MethodRegistry::~MethodRegistry()
{
    Release();
}

void MethodRegistry::__Construct()
{

}

void MethodRegistry::_Initialize()
{
}

void MethodRegistry::AddMethod(ScriptObject* method)
{
    assert(method->GetType() == ScriptObjectType::Method && "Attempted to register an invalid method!");

    _Methods[method->Guid()] = to_method(method);
}

void MethodRegistry::AddCallbackMethod(const char* name, const SymplMethodCallback& callback, MethodReturnType returnType)
{
}

ScriptObject* MethodRegistry::FindMethod(const char* name)
{
    for (auto method : _Methods) {
        if (method.second->GetName() == name) {
            return method.second.Ptr();
        }
    }
    return &ScriptObject::Empty;
}

bool MethodRegistry::TryFindMethod(const char* name, ScriptObject*& method)
{
    method = FindMethod(name);
    return (!method->IsEmpty());
}

bool MethodRegistry::Release()
{
    _Methods.clear();
    return true;
}