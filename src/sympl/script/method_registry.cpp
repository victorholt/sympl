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
#include <sympl/script/interpreter.h>

//#include <sympl/script/methods/if_method.h>
//#include <sympl/script/methods/while_method.h>
#include <sympl/script/methods/callback_method.h>

#include <sympl/util/profiler.h>
sympl_namespaces

MethodRegistry::MethodRegistry()
{

}

void MethodRegistry::__Construct()
{

}

void MethodRegistry::_Initialize()
{
    // Add the if method.
//    auto ifMethod = mem_alloc_ref(IfMethod);
//    ScriptVMInstance->AddObject(ifMethod);
//    AddMethod(ifMethod);
//
//    // Add the while method.
//    auto whileMethod = mem_alloc_ref(WhileMethod);
//    ScriptVMInstance->AddObject(whileMethod);
//    AddMethod(whileMethod);

    // Prints text.
    AddCallbackMethod("print", [](ScriptMethodArgs args) {
        std::cout << "printing something" << std::endl;
        if (!args.empty()) {
            if (args.size() > 1 && args[1].GetBool()) {
                std::cout << args[0].AsString() << " (" << args[0].GetTypeAsString() << ")";
            } else {
                std::cout << args[0].AsString();
            }
        }
    });

    // Prints text on a new line.
    AddCallbackMethod("printl", [](ScriptMethodArgs args) {
        std::cout << "print lining something" << std::endl;
        if (!args.empty()) {
            if (args.size() > 1 && args[1].GetBool()) {
                std::cout << args[0].AsString() << " (" << args[0].GetTypeAsString() << ")" << std::endl;
            } else {
                std::cout << args[0].AsString() << std::endl;
            }
        }
    });
}

void MethodRegistry::AddMethod(ScriptObject* method)
{
    sympl_assert(method->GetType() == ScriptObjectType::Method, "Attempted to register an invalid method!");
    _Methods[method->GetName()] = to_method(method);
}

void MethodRegistry::AddCallbackMethod(const char* name, const ScriptMethodCallback& callback)
{
    auto methodObj = FindMethod(name);
    if (!methodObj->IsEmpty()) {
        return;
    }

    auto method = ScriptVMInstance.CreateObjectAndInitialize<CallbackMethod>(name);
    method->SetCallback(callback);

    AddMethod(method);
}

ScriptObject* MethodRegistry::FindMethod(const char* name)
{
    for (auto& method : _Methods) {
        if (strcmp(method.second->GetName().c_str(), name) == 0) {
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
