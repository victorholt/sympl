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
#include <sympl/script/methods/while_method.h>
sympl_namespaces

WhileMethod::WhileMethod()
{
    __Construct();
}

void WhileMethod::__Construct()
{
    AddArg("__arg__");
}

Variant WhileMethod::Evaluate(ScriptMethodArgs args, ScriptObject* caller)
{
    auto method = to_method(caller);

    Variant value = args[0];
    if (value.GetType() == VariantType::Object) {
        value = dynamic_cast<ScriptObject*>(args[0].GetObject())->GetValue();
    }

    // Process the statements.
    if (value.GetType() == VariantType::Bool && value.GetBool()) {
        SharedPtr<StatementResolver> resolver = mem_alloc_ref(StatementResolver);
        resolver->SetCache(true);

        while (value.GetType() == VariantType::Bool && value.GetBool()) {
            if (caller->IsMethod()) {
                method->ProcessCallStatements();
                value = resolver->Resolve(method->GetMethodArg(0), this);
            } else {
                sympl_assert(false, "Illegal call to while loop!");
            }
        }
    }

    return Variant::Empty;
}
