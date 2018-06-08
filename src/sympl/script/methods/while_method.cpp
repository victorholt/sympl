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
#include <sympl/script/methods/while_method.h>
#include <sympl/script/statement_resolver.h>
sympl_namespaces

WhileMethod::WhileMethod()
{
    _Name = "while";

    // This method does not return anything.
    _ReturnType = MethodReturnType::Void;

    // This method will be cloned and called immediately
    // when the Interpreter processes it.
    _IsImmediate = true;

    // Skip type checking this method.
    _IgnoreReturnTypeCheck = true;
}

void WhileMethod::_Initialize(const char* name, const char* path, ScriptObject* parent)
{
    ScriptMethod::_Initialize(name, path, parent);

    // Create the argument that the 'if' call takes.
    auto arg = ScriptVMInstance->CreateObject(SYMPL_METHOD_ARG_NAME, ScriptObjectType::Object, GetScope());
    AddArg(arg);
}

Variant WhileMethod::Evaluate(const Urho3D::PODVector<Variant>& args)
{
    _CopyArgs(args);
    _ProcessArgStatements();
    Variant value = _Args[0]->GetValue();

    // Process the statements.
    if (value.GetType() == VariantType::Bool && value.GetBool()) {
        SharedPtr<StatementResolver> resolver = mem_alloc_ref(StatementResolver);
        while (value.GetType() == VariantType::Bool && value.GetBool()) {
            _ProcessCallStatements();
            value = resolver->Resolve(_ArgStringNoParenth->CStr(), this, true);
        }
    }

    return _Value;
}

ScriptObject* WhileMethod::_OnCloneCreateObject(const std::string& name, ScriptObject* parent)
{
    ScriptObject* clone = mem_alloc_ref(WhileMethod);
    clone->SetName(name);
    ScriptVMInstance->AddObject(clone, parent);
    return clone;
}
