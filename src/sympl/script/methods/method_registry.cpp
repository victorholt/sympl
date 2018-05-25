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
#include <sympl/script/methods/method_registry.h>
#include <sympl/script/methods/if_method.h>
#include <sympl/script/methods/print_method.h>
#include <sympl/script/methods/callback_method.h>

#include <sympl/script/sympl_vm.h>
sympl_namespaces

MethodRegistry::MethodRegistry()
{

}

MethodRegistry::~MethodRegistry()
{
    _Methods.clear();
}

void MethodRegistry::_Initialize()
{
    // Add the if method.
    auto ifMethod = alloc_ref(IfMethod);
    SymplVMInstance->AddObject(ifMethod);
    AddMethod(ifMethod);

    // Add the print method.
    auto printMethod = alloc_ref(PrintMethod);
    SymplVMInstance->AddObject(printMethod);
    AddMethod(printMethod);

    auto printLineMethod = alloc_ref(PrintLineMethod);
    SymplVMInstance->AddObject(printLineMethod);
    AddMethod(printLineMethod);

    // Prints the memory allocated currently.
    AddCallbackMethod("print_memory", [&](const std::vector<WeakRef<ScriptObject>>& args) {
        std::cout << "Memory Allocated (" << AllocInstance->GetMemAllocated() << ")" << std::endl;
    });
}

void MethodRegistry::AddMethod(ScriptObject* method)
{
    assert(method->GetType() == ScriptObjectType::Method && "Attempted to register an invalid method!");

    _Methods[method->Guid()] = to_method(method);
}

void MethodRegistry::AddCallbackMethod(const char* name, SymplMethodCallback callback, MethodReturnType returnType)
{
    CallbackMethod* method = static_cast<CallbackMethod*>(FindMethod(name));
    if (!method->IsEmpty()) {
        return;
    }

    method = alloc_ref(CallbackMethod);
    method->SetName(name);
    method->SetReturnType(returnType);
    method->SetCallback(callback);

    SymplVMInstance->AddObject(method);

    _Methods[method->Guid()] = method;
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
