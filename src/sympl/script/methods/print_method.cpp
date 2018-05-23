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
#include <sympl/script/sympl_vm.h>
#include <sympl/script/methods/print_method.h>

#include <fmt/format.h>
sympl_namespaces

PrintMethod::PrintMethod()
{
    _Name = "print";

    // This method will be cloned and called immediately
    // when the Interpreter processes it.
    _IsImmediate = true;

    // This method does not return anything.
    _ReturnType = MethodReturnType::Void;

    // Skip type checking this method.
    _IgnoreReturnTypeCheck = true;
}

void PrintMethod::_Initialize(const char* name, const char* path, ScriptObject* parent)
{
    ScriptObject::_Initialize(name, path, parent);

    // Create the scope and the argument that the 'if' call takes.
    auto scope = SymplVMInstance->CreateObject(".", ScriptObjectType::Object, this);
    auto arg = SymplVMInstance->CreateObject("__arg__", ScriptObjectType::Object, scope);
    AddArg(arg);
}

Variant PrintMethod::Evaluate(const std::vector<Variant>& args)
{
    _CopyArgs(args);
    _ProcessArgStatements();

    Variant value = _Args[0]->GetValue();

    // Remove the string token.
    if (value.GetType() == VariantType::StringBuffer) {
        value.GetStringBuffer()->Replace(SYMPL_STRING_TOKEN, "");
    }

    if (value.GetType() == VariantType::ScriptObject) {
        std::cout << value.AsString() << fmt::format(" ({0}) ", value.GetTypeAsString());
    } else {
        std::cout << value.AsString();
    }

    return Variant::Empty;
}

ScriptObject* PrintMethod::_OnCloneCreateObject(const std::string& name, ScriptObject* parent)
{
    ScriptObject* clone = alloc_ref(PrintMethod);
    clone->SetName(name);
    SymplVMInstance->AddObject(clone, parent);
    return clone;
}

///////////////////////////////////////////////////////////////////
// PRINT LINE
///////////////////////////////////////////////////////////////////

PrintLineMethod::PrintLineMethod()
{
    _Name = "printl";

    // This method will be cloned and called immediately
    // when the Interpreter processes it.
    _IsImmediate = true;

    // This method does not return anything.
    _ReturnType = MethodReturnType::Void;

    // Skip type checking this method.
    _IgnoreReturnTypeCheck = true;
}

void PrintLineMethod::_Initialize(const char* name, const char* path, ScriptObject* parent)
{
    ScriptObject::_Initialize(name, path, parent);

    // Create the scope and the argument that the 'if' call takes.
    auto scope = SymplVMInstance->CreateObject(".", ScriptObjectType::Object, this);
    auto arg = SymplVMInstance->CreateObject("__arg__", ScriptObjectType::Object, scope);
    AddArg(arg);
}

Variant PrintLineMethod::Evaluate(const std::vector<Variant>& args)
{
    _CopyArgs(args);
    _ProcessArgStatements();

    Variant value = _Args[0]->GetValue();

    // Remove the string token.
    if (value.GetType() == VariantType::StringBuffer) {
        value.GetStringBuffer()->Replace(SYMPL_STRING_TOKEN, "");
    }

    if (value.GetType() == VariantType::ScriptObject) {
        std::cout << value.AsString() << fmt::format(" ({0}) ", value.GetTypeAsString()) << std::endl;
    } else {
        std::cout << value.AsString() << std::endl;
    }

    return Variant::Empty;
}

ScriptObject* PrintLineMethod::_OnCloneCreateObject(const std::string& name, ScriptObject* parent)
{
    ScriptObject* clone = alloc_ref(PrintLineMethod);
    clone->SetName(name);
    SymplVMInstance->AddObject(clone, parent);
    return clone;
}
