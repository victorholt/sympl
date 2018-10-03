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
#include <sympl/script/resolvers/eval_resolver.h>
#include <sympl/script/statement_resolver.h>
sympl_namespaces

Variant EvalResolver::GetEvalFromStatementBuffer(const char* stmtStr, ScriptObject* scriptObject)
{
    if (strlen(stmtStr) <= 0 && scriptObject->GetType() != ScriptObjectType::Method) {
        sympl_assert(false, "Attempting to evaluate an invalid statement!");
    }

    // Create the statement and set the string.
    SharedPtr<StatementResolver> stmtResolver = mem_alloc_ref(StatementResolver);
    return stmtResolver->Resolve(stmtStr, scriptObject);
}

bool EvalResolver::IsStatementBufferTrue(const char* stmtStr, ScriptObject* scriptObject)
{
    Variant evalValue = GetEvalFromStatementBuffer(stmtStr, scriptObject);
    if (evalValue.GetType() == VariantType::Int && evalValue.GetInt() != 0) {
        return true;
    } else if (evalValue.GetType() == VariantType::Float && evalValue.GetFloat() != 0.0f) {
        return true;
    } else if (evalValue.GetType() == VariantType::StringBuffer && evalValue.GetStringBuffer()->Length() > 0) {
        return true;
    } else if (evalValue.GetType() == VariantType::Bool && evalValue.GetBool()) {
        return true;
    } else if (evalValue.GetType() == VariantType::Object && !to_script_object(evalValue)->IsEmpty()) {
        return true;
    }
    return false;
}

