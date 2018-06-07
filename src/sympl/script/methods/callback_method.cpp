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
#include <sympl/script/script_vm.h>
#include <sympl/script/methods/callback_method.h>

#include <fmt/format.h>
sympl_namespaces

CallbackMethod::CallbackMethod()
{
    _Name = "callback_method";

    // This method does not return anything.
    _ReturnType = MethodReturnType::Void;

    // This method will be cloned and called immediately
    // when the Interpreter processes it.
    _IsImmediate = true;

    // Skip type checking this method.
    _IgnoreReturnTypeCheck = true;
}

void CallbackMethod::_Initialize(const char* name, const char* path, ScriptObject* parent)
{
    ScriptMethod::_Initialize(name, path, parent);
}

Variant CallbackMethod::Evaluate(const Urho3D::PODVector<Variant>& args)
{
    _CopyArgs(args);
    _ProcessArgStatements();

    if (_Callback) {
        _Callback(_Args);
    }

    return Variant::Empty;
}

void CallbackMethod::_CopyArgs(const Urho3D::PODVector<Variant>& args)
{
    if (args.Empty()) return;

    int argIndex = 0;
    for (auto& argIt : args) {
        if (_Args.Size() <= argIndex) {
            auto arg = ScriptVMInstance->CreateObject(fmt::format("__arg__{0}", argIndex).c_str(),
                                                      ScriptObjectType::Object, _Scope.Ptr());
            arg->SetValue(argIt);
            AddArg(arg);
        } else {
            _Args[argIndex]->SetValue(argIt);
        }
        argIndex++;
    }
}

ScriptObject* CallbackMethod::_OnCloneCreateObject(const std::string& name, ScriptObject* parent)
{
    ScriptObject* clone = mem_alloc(CallbackMethod);
    clone->SetName(name);
    dynamic_cast<CallbackMethod*>(clone)->SetCallback(_Callback);
    ScriptVMInstance->AddObject(clone, parent);
    return clone;
}
