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
#include <sympl/script/resolvers/method_resolver.h>
#include <sympl/script/resolvers/eval_resolver.h>
#include <sympl/script/script_vm.h>
#include <sympl/script/script_method.h>
#include <sympl/script/method_cache_entry.h>
#include <sympl/script/cache/script_cache.h>
#include <sympl/script/cache/script_cache_object.h>
sympl_namespaces

Variant MethodResolver::Resolve(StatementResolver* stmtResolver, StringBuffer* currentStr,
                               ScriptObject* destObject, StatementOperator op)
{
    sympl_assert(destObject->IsMethod(), "Non-method call attempt!");

    currentStr->Clear();

    std::vector<Variant> args;
    _FindArguments(stmtResolver, destObject, args, !_IsRecursiveCall(destObject));

    return to_method(destObject)->Evaluate(args, _Caller);
}

bool MethodResolver::_FindArguments(StatementResolver* stmtResolver, ScriptObject* destObject, std::vector<Variant>& args, bool useCache)
{
    char currentChar = '\0';
    int parethNestLevel = 1; // We're initially inside of a parameter.
    bool recording = false;

    SharedPtr<StringBuffer> methodArg = mem_alloc_ref(StringBuffer);

    auto statementStr = stmtResolver->GetStatementString();
    size_t charLocation = stmtResolver->GetCharLocation();

    // Cache arguments for optimization.
    ScriptCacheObject* cacheObject = nullptr;
    std::string cacheKey;

    if (useCache) {
        cacheObject = ScriptCacheInstance.Fetch(destObject);
        cacheKey = statementStr->CStr();
        std::vector<Object*>* cacheEntries;

        if (cacheObject->HasKey(cacheKey)) {
            if (cacheObject->FetchValues(cacheKey, cacheEntries)) {
                for (const auto& entry : *cacheEntries) {
                    auto methodEntry = dynamic_cast<MethodCacheEntry*>(entry);
                    args.emplace_back(methodEntry->Value);
                    stmtResolver->SetCharLocation(methodEntry->StmtCharLocation);
                }
                return true;
            }
        }
    }

    while (charLocation < statementStr->Length()) {
        currentChar = statementStr->Get(charLocation);
        charLocation++;

        stmtResolver->SetCharLocation(charLocation);

        // Check if we're starting a string.
        if (currentChar == '%' && statementStr->PeekSearch(SYMPL_STRING_TOKEN, charLocation - 1)) {
            recording = !recording;
            methodArg->Append(SYMPL_STRING_TOKEN);
            charLocation = charLocation + strlen(SYMPL_STRING_TOKEN) - 1;
            continue;
        }

        if (!recording) {
            if (currentChar == '(') {
                parethNestLevel++;
            }
            if (currentChar == ')') {
                parethNestLevel--;
            }

            if (currentChar == ',' || (currentChar == ')' && parethNestLevel == 0)) {
                EvalResolver evalResolver;

                // Cache the argument.
                if (useCache) {
                    auto cacheEntry = mem_alloc_ref(MethodCacheEntry);
                    cacheEntry->Value = evalResolver.GetEvalFromStatementBuffer(methodArg->CStr(), destObject);
                    cacheEntry->StmtCharLocation = charLocation;
                    cacheObject->Store(cacheKey, cacheEntry);
                    args.emplace_back(cacheEntry->Value);
                } else {
                    args.emplace_back(evalResolver.GetEvalFromStatementBuffer(methodArg->CStr(), destObject));
                }
                methodArg->Clear();

                if (currentChar == ')') {
                    return true;
                }

            } else if (currentChar != ')') {
                if (currentChar == '#' && methodArg->Length() != 0) {
                    methodArg->AppendByte(currentChar);
                } else if (currentChar != '#') {
                    methodArg->AppendByte(currentChar);
                }
            }
        } else {
            methodArg->AppendByte(currentChar);
        }
    }

    return false;
//    sympl_assert(false, "Missing closing ')' when attempting to resolve method!");
}

bool MethodResolver::_IsRecursiveCall(ScriptObject* destObject)
{
    auto parent = _Caller->GetParent();
    if (!parent.IsValid() || parent.Ptr() == ScriptVMInstance.GetGlobalObject()) {
        return false;
    }

    while (parent.IsValid() && parent.Ptr() != ScriptVMInstance.GetGlobalObject()) {
        if (parent.Ptr() == destObject) {
            return true;
        }
        parent = parent->GetParent();
    }

    return false;
}
