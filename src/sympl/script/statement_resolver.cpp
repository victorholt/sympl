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
#include <sympl/script/statement_resolver.h>
#include <sympl/script/script_vm.h>
#include <sympl/script/method_registry.h>
#include <sympl/script/script_array.h>
#include <sympl/util/number_helper.h>
#include <sympl/util/string_helper.h>
#include <sympl/script/statement_cache.h>
sympl_namespaces

Variant EvalResolver::GetEvalFromStatementBuffer(const char* stmtStr, ScriptObject* scriptObject)
{
    if (strlen(stmtStr) <= 0 && scriptObject->GetType() != ScriptObjectType::Method) {
        sympl_assert(false && "Attempting to evaluate an invalid statement!");
    }

    // Create the statement and set the string.
    SharedPtr<StatementResolver> stmtResolver = SymplRegistry.Get<StatementResolver>();
    if (stmtResolver->GetInUse()) {
        stmtResolver = mem_alloc_ref(StatementResolver);
    }
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
    } else if (evalValue.GetType() == VariantType::Object && !variant_script_object(evalValue)->IsEmpty()) {
        return true;
    }
    return false;
}

Variant MethodResolver::Resolve(StatementResolver* stmtResolver, StringBuffer* currentStr,
                ScriptObject* varObject, StatementOperator op)
{
    auto tokenHelper = ScriptVMInstance->GetScriptToken();
    auto statementStr = stmtResolver->GetStatementString();

    // Save args in the variant.
    Urho3D::PODVector<Variant> args;

    // Check if this is really a valid method object.
    ScriptObject* scriptObject = varObject;

    // Check if we're attempting to allocate a new object.
    if (varObject->GetType() != ScriptObjectType::Method) {
        if (currentStr->Contains('.')) {
            scriptObject = ScriptVMInstance->FindObjectByPath(
                    currentStr->CStr(),
                    varObject->GetParent().IsValid() ? varObject->GetParent().Ptr() : nullptr
            );
        } else {
            scriptObject = ScriptVMInstance->GetMethodRegistry()->FindMethod(currentStr->CStr());
        }
    }

    // Ensure this is a proper method to resolve.
    if (IsNullObject(scriptObject) || scriptObject->IsEmpty() || scriptObject->GetType() != ScriptObjectType::Method) {
        return Variant::Empty;
    }

    // Remove the name of the method so it's not part of the statement value.
    if (currentStr->Contains('.') && scriptObject->GetParent().IsValid() && scriptObject->GetParent()->GetParent().IsValid()) {
        std::string methodName = scriptObject->GetParent()->GetParent()->GetCleanName();
        methodName.append(".");
        methodName.append(scriptObject->GetCleanName());
        currentStr->Replace(methodName.c_str(), "");
    } else {
        currentStr->Replace(scriptObject->GetCleanName().c_str(), "");
    }

    // Check if we have arguments cached.
    MethodArgCache* methodArgCache = nullptr;

    // Attempt to detect a recursive function, which can't utilize
    // method caching.
    if (!to_method(scriptObject)->GetIsRecursive()) {
        methodArgCache = to_method(scriptObject)->FindOrCreateArgCache(stmtResolver->GetStatementString()->CStr());
    }
    if (!IsNullObject(methodArgCache) && methodArgCache->Args.size() == to_method(scriptObject)->GetNumArgs()) {
        for (auto& cacheArg: methodArgCache->Args) {
            if (cacheArg.ArgValue.GetType() == VariantType::Object) {
                auto obj = ((ScriptObject*)cacheArg.ArgValue.GetObject());
                args.Push(obj->GetValue());

                // Advance the character location.
                stmtResolver->SetCurrentCharLocation(stmtResolver->GetCurrentCharLocation() + cacheArg.ArgStringLength);
            } else {
                args.Push(cacheArg.ArgValue);

                // Advance the character location.
                stmtResolver->SetCurrentCharLocation(stmtResolver->GetCurrentCharLocation() + cacheArg.ArgStringLength);
            }
        }

        auto retVal = to_method(scriptObject)->Evaluate(args);
        return retVal;
    }

    // Parse out the object value.
    char currentChar = '\0';
    char nextChar = '\0';
    bool recording = false;

    // String to resolve.
    auto resolveStr = mem_alloc_ref(StringBuffer);

    // Current/last concatenating condition.
    std::string currentConcatConditionStr;
    std::string lastConcatConditionStr;

    // Current argument value.
    Variant argValue;

    // Build out the argument statements.
    auto stmtLen = statementStr->Length();
    int argCacheLength = 0;
    while (stmtResolver->GetCurrentCharLocation() < stmtLen) {
        currentChar = statementStr->Get(stmtResolver->GetCurrentCharLocation());
        nextChar = statementStr->Get(stmtResolver->GetCurrentCharLocation() + 1);
        stmtResolver->SetCurrentCharLocation(stmtResolver->GetCurrentCharLocation() + 1);
        argCacheLength++;

//        if (currentChar == '%' && statementStr->PeekSearch(SYMPL_STRING_TOKEN, stmtResolver->GetCurrentCharLocation() - 1)) {
//            resolveStr->Append(SYMPL_STRING_TOKEN);
//            stmtResolver->SetCurrentCharLocation(stmtResolver->GetCurrentCharLocation() + strlen(SYMPL_STRING_TOKEN) - 1);
//            recording = !recording;
//            continue;
//        }

        if (!recording && currentChar == '(') {
            // Check to see if the varObject we have give is a method. If it's coming from
            // the Interpreter it may be a method and we don't want to skip it!
            if (resolveStr->Empty() && varObject->GetType() == ScriptObjectType::Method) {
                auto methodResolver = SymplRegistry.Get<MethodResolver>();
                resolveStr->Append(methodResolver->Resolve(stmtResolver, resolveStr, varObject, op).AsString());
            } else {
                // Determine if the current statement buffer is an existing method.
                ScriptObject* existingMethod = nullptr;
                if (resolveStr->Contains('.') && varObject->GetParent().IsValid()) {
                    auto scope = varObject->GetParent();
                    existingMethod = ScriptVMInstance->FindObjectByPath(resolveStr->CStr(), scope.Ptr());
                } else {
                    existingMethod = ScriptVMInstance->GetMethodRegistry()->FindMethod(resolveStr->CStr());
                }

                if (!existingMethod->IsEmpty() && existingMethod->GetType() == ScriptObjectType::Method) {
                    auto methodResolver = SymplRegistry.Get<MethodResolver>();
                    resolveStr->Append(methodResolver->Resolve(stmtResolver, resolveStr, existingMethod, op).AsString());
                } else {
                    auto parenthResolver = SymplRegistry.Get<ParenthResolver>();
                    resolveStr->Append(parenthResolver->Resolve(stmtResolver, resolveStr, varObject, op).AsString());
                }
            }
            continue;
        }

        // Check if we're dealing with a double operator for concatenating comparisons.
        if (!recording && tokenHelper->IsOperator(currentChar) && tokenHelper->IsOperator(nextChar)) {
            bool isConcatOperator = false;
            EvalResolver evalResolver;

            if ((currentChar == '&' && nextChar == '&') || (currentChar == '|' && nextChar == '|')) {
                isConcatOperator = true;
                currentConcatConditionStr.clear();
                currentConcatConditionStr.append(1, currentChar);
                currentConcatConditionStr.append(1, nextChar);

                // If this is the first time encountering the concat condition,
                // then evaluate the current argument and move on to the next
                // iteration.
                if (lastConcatConditionStr.empty()) {
                    argValue = evalResolver.IsStatementBufferTrue(resolveStr->CStr(), scriptObject);
                    lastConcatConditionStr = currentConcatConditionStr;
                    continue;
                }
            }

            // Check if we need to ensure that the entire statement is true.
            if (isConcatOperator) {
                if (lastConcatConditionStr == "&&" && argValue.GetBool()) {
                    argValue = evalResolver.IsStatementBufferTrue(resolveStr->CStr(), scriptObject);
                    if (!argValue.GetBool()) {
                        argValue = false;
                    }
                } else if (lastConcatConditionStr == "||" && !argValue.GetBool()) {
                    argValue = evalResolver.IsStatementBufferTrue(resolveStr->CStr(), scriptObject);
                }

                lastConcatConditionStr = currentConcatConditionStr;
                stmtResolver->SetCurrentCharLocation(stmtResolver->GetCurrentCharLocation() + 1);
                continue;
            }
        }

        // Build and evaluate the statement.
        if (!recording && currentChar == ',' || (currentChar == ')')) {
            EvalResolver evalResolver;

            // Check if the string is an object/value and not a buffer.
            Variant objectValue = stmtResolver->_IsNumber(resolveStr->CStr());

            // Check if the value is a boolean value.
            if (objectValue.IsEmpty()) {
                objectValue = stmtResolver->_IsBoolean(resolveStr->CStr());
            }

            // Check to see if we're an object or string.
            ScriptObject *obj = nullptr;
            if (objectValue.IsEmpty()) {
                if (varObject->GetType() == ScriptObjectType::Method) {
                    // Check if our string is structured like a path.
                    if (resolveStr->Contains('.') && varObject->GetParent().IsValid()) {
                        auto scope = varObject->GetParent();
                        obj = ScriptVMInstance->FindObjectByPath(resolveStr->CStr(), scope.Ptr());
                    } else {
                        obj = to_method(varObject)->GetScope()->GetContext()->FindVariable(resolveStr->CStr(), true);
                    }
                } else {
                    obj = varObject->GetContext()->FindVariable(resolveStr->CStr(), true);
                }

                if (!obj->IsEmpty()) {
                    if (obj->GetType() == ScriptObjectType::Object || obj->GetType() == ScriptObjectType::Method || obj->IsClass()) {
                        objectValue = obj;
                    } else {
                        objectValue = obj->GetValue();
                    }
                }
            }

            // Check if we were able to find an object value.
            MethodArgCacheValue cacheValue;
            cacheValue.ArgStringLength = argCacheLength;
            argCacheLength = 0;

            if (!objectValue.IsEmpty()) {
                args.Push(objectValue);

                if (!IsNullObject(methodArgCache)) {
                    if (!IsNullObject(obj)) {
                        cacheValue.ArgValue = obj;
                        methodArgCache->Args.push_back(cacheValue);
                    } else {
                        cacheValue.ArgValue = objectValue;
                        methodArgCache->Args.push_back(cacheValue);
                    }
                }
            } else {
                // Check if we already have an argument value.
                if (argValue.IsEmpty()) {
                    argValue = evalResolver.GetEvalFromStatementBuffer(resolveStr->CStr(), scriptObject);
                    args.Push(argValue);
                } else if (currentConcatConditionStr == "&&") {
                    if (argValue.GetType() != VariantType::Bool || argValue.GetBool()) {
                        argValue = evalResolver.GetEvalFromStatementBuffer(resolveStr->CStr(), scriptObject);
                    }
                    args.Push(argValue);
                } else if (currentConcatConditionStr == "||") {
                    if (argValue.GetType() != VariantType::Bool || !argValue.GetBool()) {
                        argValue = evalResolver.GetEvalFromStatementBuffer(resolveStr->CStr(), scriptObject);
                    }
                    args.Push(argValue);
                }

                if (!IsNullObject(methodArgCache)) {
                    cacheValue.ArgValue = argValue;
                    methodArgCache->Args.push_back(cacheValue);
                }
            }

            argValue = Variant::Empty;
            currentConcatConditionStr = "";
            lastConcatConditionStr = "";
            resolveStr->Clear();

            // We've completed building out the arguments.
            if (currentChar == ')') {
                mem_free_ref(StringBuffer, resolveStr);

                // Restore the statement buffer.
                //_StatementBuffer->Append(savedStatementStr);
                auto retVal = to_method(scriptObject)->Evaluate(args);

                args.Clear();
                return retVal;
            }
        }

        if (recording || (currentChar != ',' && currentChar != '#')) {
            resolveStr->Append(std::string(1, currentChar));
        }
    }

    // We failed to exit the while loop early!
    sympl_assert(false && "Unclosed call to method!");
    return Variant::Empty;
}

Variant ParenthResolver::Resolve(StatementResolver* stmtResolver, StringBuffer* currentStr,
                    ScriptObject* varObject, StatementOperator op)
{
    auto tokenHelper = ScriptVMInstance->GetScriptToken();
    auto statementStr = stmtResolver->GetStatementString();

    // Parse out the object value.
    char currentChar = '\0';
    char nextChar = '\0';
    bool recording = false;

    // String to resolve.
    auto resolveStr = mem_alloc_ref(StringBuffer);

    // Current/last concatenating condition.
    std::string currentConcatConditionStr;
    std::string lastConcatConditionStr;

    // Current return value.
    Variant retValue;

    // Build out the argument statements.
    while (stmtResolver->GetCurrentCharLocation() < statementStr->Length()) {
        currentChar = statementStr->Get(stmtResolver->GetCurrentCharLocation());
        nextChar = statementStr->Get(stmtResolver->GetCurrentCharLocation() + 1);
        stmtResolver->SetCurrentCharLocation(stmtResolver->GetCurrentCharLocation() + 1);

//        if (currentChar == '%' && statementStr->PeekSearch(SYMPL_STRING_TOKEN, stmtResolver->GetCurrentCharLocation() - 1)) {
//            resolveStr->Append(SYMPL_STRING_TOKEN);
//            stmtResolver->SetCurrentCharLocation(stmtResolver->GetCurrentCharLocation() + strlen(SYMPL_STRING_TOKEN) - 1);
//            recording = !recording;
//            continue;
//        }

        // Resolve another contained value.
        if (!recording && currentChar == '(') {
            // Determine if the current statement buffer is an existing method.
            ScriptObject* existingMethod = nullptr;
            if (resolveStr->Contains('.')) {
                existingMethod = ScriptVMInstance->FindObjectByPath(
                        resolveStr->CStr(),
                        varObject->GetParent().IsValid() ? varObject->GetParent().Ptr() : nullptr
                );
            } else {
                existingMethod = ScriptVMInstance->GetMethodRegistry()->FindMethod(resolveStr->CStr());
            }

            if (!existingMethod->IsEmpty() && existingMethod->GetType() == ScriptObjectType::Method) {
                auto methodResolver = SymplRegistry.Get<MethodResolver>();
                resolveStr->Append(methodResolver->Resolve(stmtResolver, resolveStr, existingMethod, op).AsString());
            } else {
                auto parenthResolver = SymplRegistry.Get<ParenthResolver>();
                resolveStr->Append(parenthResolver->Resolve(stmtResolver, resolveStr, varObject, op).AsString());
            }
            continue;
        }

        // Check if we're dealing with a double operator for concatenating comparisons.
        if (!recording && tokenHelper->IsOperator(currentChar) && tokenHelper->IsOperator(nextChar)) {
            EvalResolver evalResolver;
            bool isConcatOperator = false;

            if ((currentChar == '&' && nextChar == '&') || (currentChar == '|' && nextChar == '|')) {
                isConcatOperator = true;
                currentConcatConditionStr.clear();
                currentConcatConditionStr.append(1, currentChar);
                currentConcatConditionStr.append(1, nextChar);

                // If this is the first time encountering the concat condition,
                // then evaluate the current argument and move on to the next
                // iteration.
                if (lastConcatConditionStr.size() == 0) {
                    retValue = evalResolver.IsStatementBufferTrue(resolveStr->CStr(), varObject);
                    lastConcatConditionStr = currentConcatConditionStr;
                    continue;
                }
            }

            // Check if we need to ensure that the entire statement is true.
            if (isConcatOperator) {
                if (lastConcatConditionStr == "&&" && retValue.GetBool()) {
                    retValue = evalResolver.IsStatementBufferTrue(resolveStr->CStr(), varObject);
                    if (!retValue.GetBool()) {
                        retValue = false;
                    }
                } else if (lastConcatConditionStr == "||" && !retValue.GetBool()) {
                    retValue = evalResolver.IsStatementBufferTrue(resolveStr->CStr(), varObject);
                }

                lastConcatConditionStr = currentConcatConditionStr;
                stmtResolver->SetCurrentCharLocation(stmtResolver->GetCurrentCharLocation() + 1);
                continue;
            }
        }

        // Build and evaluate the statement.
        if (!recording && currentChar == ')') {
            EvalResolver evalResolver;

            // Check if we already have an argument value.
            if (retValue.IsEmpty()) {
                retValue = evalResolver.GetEvalFromStatementBuffer(resolveStr->CStr(), varObject);
            } else if (currentConcatConditionStr == "&&") {
                if (retValue.GetType() != VariantType::Bool || retValue.GetBool()) {
                    retValue = evalResolver.GetEvalFromStatementBuffer(resolveStr->CStr(), varObject);
                }
            } else if (currentConcatConditionStr == "||") {
                if (retValue.GetType() != VariantType::Bool || !retValue.GetBool()) {
                    retValue = evalResolver.GetEvalFromStatementBuffer(resolveStr->CStr(), varObject);
                }
            }

            // Restore the statement buffer.
//                resolveStr->Clear();
//                resolveStr->Append(savedStatementStr);

            // Return our value.
            mem_free_ref(StringBuffer, resolveStr);
            return retValue;
        }

        if (recording || (!recording && currentChar != '#')) {
            resolveStr->AppendByte(currentChar);
        }
    }

    // We failed to exit the while loop early!
    sympl_assert(false && "Unclosed parenthesis in statement found!");
    return Variant::Empty;
}

bool ArrayResolver::_CheckValueAssignment(StatementResolver* stmtResolver, ScriptObject* varObject, Variant& index)
{
    auto statementStr = stmtResolver->GetStatementString();
    SharedPtr<StringBuffer> resolveStr = mem_alloc_ref(StringBuffer);

    // Quick check to see if the equal sign occurs before or after our brackets.
    // This will determine whether we're assigning a value to an array item.
    auto bracketIndex = statementStr->FirstOccurrence('[', 0);
    auto opIndex = statementStr->FirstOccurrence('=', 0);

    // We have a problem if we can't find an index for either of these.
    if (bracketIndex == -1 || opIndex == -1) {
        sympl_assert(false && "Invalid array statement found!");
    }

    // If we're creating an array then the brackets will occur after our '=' sign.
    if (bracketIndex > opIndex) {
        return false;
    }

    // Find the index of the array we want to modify.
    char currentChar = '\0';
    bool recording = false;

    while (true) {
        currentChar = statementStr->Get(stmtResolver->GetCurrentCharLocation());
        stmtResolver->SetCurrentCharLocation(stmtResolver->GetCurrentCharLocation() + 1);

        // Break when we reach the value assignment.
        if (currentChar == '=') {
            break;
        }

        // Skip bracket beginning.
        if (currentChar == '[') {
            continue;
        }

        // Check if we've reached the end.
        if (currentChar == '\0') {
            sympl_assert(false && "Detected unclosed bracket in array assignment!");
        }

        // Check if we've encountered a quote.
        if (currentChar == '%' &&
            statementStr->PeekSearch(SYMPL_STRING_TOKEN, stmtResolver->GetCurrentCharLocation() - 1)) {
            stmtResolver->SetCurrentCharLocation(
                    stmtResolver->GetCurrentCharLocation() + strlen(SYMPL_STRING_TOKEN) - 1);
            recording = !recording;
            continue;
        }

        // Skip spaces if we're not recording.
        if (!recording && currentChar == '#') {
            continue;
        }

        // Check if we need to solve the value inside the bracket.
        if (currentChar == ']') {
            EvalResolver evalResolver;
            index = evalResolver.GetEvalFromStatementBuffer(resolveStr->CStr(), varObject);
            return true;
        }

        // Record our string.
        resolveStr->AppendByte(currentChar);
    }

    return true;
}

Variant ArrayResolver::Resolve(StatementResolver* stmtResolver, StringBuffer* currentStr,
                        ScriptObject* varObject, StatementOperator op)
{
    currentStr->Clear();

    auto statementStr = stmtResolver->GetStatementString();

    // Parse out the object value.
    char currentChar = '\0';
    bool recording = false;

    // String to resolve.
    auto resolveStr = mem_alloc_ref(StringBuffer);
    SharedPtr<StringBuffer> arrayAssocKey = mem_alloc_ref(StringBuffer);

    // Check if we're an array assignment rather than
    // just an array declaration.
    Variant valueIndex;
    bool valueAssignment = _CheckValueAssignment(stmtResolver, varObject, valueIndex);
    bool isAssocArray = false;

    int arrayIndex = 0;

    // Current/last concatenating condition.
    std::string currentConcatConditionStr;
    std::string lastConcatConditionStr;

    // Current argument value.
    Variant argValue;

    // Build out the argument statements.
    while (true) {
        currentChar = statementStr->Get(stmtResolver->GetCurrentCharLocation());
        stmtResolver->SetCurrentCharLocation(stmtResolver->GetCurrentCharLocation() + 1);

        // Check if we've reached the end.
        if (currentChar == '\0') {
            break;
        }

        // Check if we've encountered a quote.
        if (currentChar == '%' && statementStr->PeekSearch(SYMPL_STRING_TOKEN, stmtResolver->GetCurrentCharLocation() - 1)) {
            stmtResolver->SetCurrentCharLocation(stmtResolver->GetCurrentCharLocation() + strlen(SYMPL_STRING_TOKEN) - 1);
            recording = !recording;
            continue;
        }

        // Check if we're an associative array.
        if (!recording && currentChar == ':') {
            isAssocArray = true;
            arrayAssocKey->Append(resolveStr);
            resolveStr->Clear();
            continue;
        }

        if (!recording && (currentChar == ']' || currentChar == ',')) {
            // Save our array entry.
            if (!resolveStr->Empty()) {
                if (isAssocArray && arrayAssocKey->Empty()) {
                    sympl_assert(false && "Missing association key in array!");
                }

                EvalResolver evalResolver;
                Variant value = evalResolver.GetEvalFromStatementBuffer(resolveStr->CStr(), varObject);

                if (!isAssocArray) {
                    valueIndex.Set(arrayIndex);
                } else {
                    valueIndex.Set(arrayAssocKey->CStr());
                }

                to_array(varObject)->SetItem(valueIndex.AsString().c_str(), value);

                resolveStr->Clear();
                arrayAssocKey->Clear();
                arrayIndex++;
            }
        } else if (currentChar == ';') {
            stmtResolver->SetCurrentCharLocation(stmtResolver->GetCurrentCharLocation() - 1);

            if (valueAssignment) {
                EvalResolver evalResolver;
                Variant value = evalResolver.GetEvalFromStatementBuffer(resolveStr->CStr(), varObject);
                to_array(varObject)->SetItem(valueIndex.AsString().c_str(), value);

                return value;
            } else {
                to_array(varObject)->SetValue(varObject);
                return Variant(varObject->GetCleanName().c_str());
            }
        } else {
            if (recording || (!recording && currentChar != '#')) {
                resolveStr->AppendByte(currentChar);
            }
        }
    }

    sympl_assert(false && "Unclosed array found!");
    return Variant::Empty;
}

Variant ArrayResolver::ResolveValue(StatementResolver* stmtResolver, StringBuffer* currentStr, ScriptObject* varObject)
{
    // Find our array object.
    auto arrayObj = varObject->GetContext()->FindVariable(currentStr->CStr());
    if (arrayObj->IsEmpty() || arrayObj->GetType() != ScriptObjectType::Array) {
        sympl_assert(false && "Illegal call to a non-existent array variable!");
    }
    currentStr->Clear();

    auto statementStr = stmtResolver->GetStatementString();
    SharedPtr<StringBuffer> resolveStr = mem_alloc_ref(StringBuffer);

    // Quick check to see if the equal sign occurs before or after our brackets.
    // This will determine whether we're assigning a value to an array item.
    auto bracketIndex = statementStr->FirstOccurrence('[', 0, 10);
    auto opIndex = statementStr->FirstOccurrence('=');

    // If opIndex does not = -1, fast forward the string.
    if (opIndex != -1) {
        stmtResolver->SetCurrentCharLocation(bracketIndex);
    }

    // We have a problem if we can't find an index for either of these.
    if (bracketIndex == -1) {
        sympl_assert(false && "Invalid array statement found in attempting to retrieve array value!");
    }

    // Find the index of the array we want to modify.
    char currentChar = '\0';
    bool recording = false;

    while (true) {
        currentChar = statementStr->Get(stmtResolver->GetCurrentCharLocation());
        stmtResolver->SetCurrentCharLocation(stmtResolver->GetCurrentCharLocation() + 1);

        // Break when we reach the value assignment.
        if (currentChar == '=') {
            break;
        }

        // Skip bracket beginning.
        if (currentChar == '[') {
            continue;
        }

        // Check if we've reached the end.
        if (currentChar == '\0') {
            sympl_assert(false && "Detected unclosed bracket in array assignment!");
        }

        // Check if we've encountered a quote.
        if (currentChar == '%' &&
            statementStr->PeekSearch(SYMPL_STRING_TOKEN, stmtResolver->GetCurrentCharLocation() - 1)) {
            stmtResolver->SetCurrentCharLocation(
                    stmtResolver->GetCurrentCharLocation() + strlen(SYMPL_STRING_TOKEN) - 1);
            recording = !recording;
            continue;
        }

        // Skip spaces if we're not recording.
        if (!recording && currentChar == '#') {
            continue;
        }

        // Check if we need to solve the value inside the bracket.
        if (currentChar == ']') {
            EvalResolver evalResolver;
            auto value = evalResolver.GetEvalFromStatementBuffer(resolveStr->CStr(), varObject);
            return to_array(arrayObj)->GetItem(value.AsString().c_str());
        }

        // Record our string.
        resolveStr->AppendByte(currentChar);
    }

    return Variant::Empty;
}

StatementResolver::StatementResolver()
{
    __Construct();
}

void StatementResolver::__Construct()
{
    _StmtString = mem_alloc_ref(StringBuffer);
    _TokenHelper = ScriptVMInstance->GetScriptToken();
}

Variant StatementResolver::Resolve(const char* cstr, ScriptObject* varObject, bool cache)
{
    // TODO: Make thread-safe?
    _InUse = true;

    // Check if statement has a method.
    bool statement_has_method = false;
    StatementCacheEntry* stmtCache = nullptr;
    ScriptContext* cacheContext = nullptr;

//    SharedPtr<StringBuffer> cleanStr = mem_alloc_ref(StringBuffer);
//    cleanStr->Append(cstr);
//    cleanStr->Replace(SYMPL_STRING_TOKEN, "\"");
//    std::string str = cleanStr->CStr();
    std::string str = cstr;

    if (!varObject->GetContext()->IsEmpty()) {
        cacheContext = varObject->GetContext();
    } else {
        cacheContext = varObject->GetContext()->GetParentContext();
    }

    if (!IsNullObject(cacheContext)) {
        cacheContext = varObject->GetContext();
        stmtCache = cacheContext->GetStatementCache()->GetEntry(str.c_str());
    }

    // Clear out the previous statement and append the new statement.
    if (IsNullObject(stmtCache) && _StmtEntries.empty()) {
        _StmtString->Clear();
        _StmtString->Append(str);
        if (_StmtString->LastByte() != ';') {
            _StmtString->AppendByte(';');
        }

        // Set our default operator.
        StatementOperator currentOp = StatementOperator::Equals;

        // Go through our string and create our entries.
        auto stmtEntryStr = mem_alloc_ref(StringBuffer);
        char currentChar = '\0';
        char nextChar = '\0';
        bool recording = false;

        auto stmtLen = _StmtString->Length();
        _CurrentCharLocation = 0;

        while (_CurrentCharLocation < _StmtString->Length()) {
            currentChar = _StmtString->Get(_CurrentCharLocation);
            nextChar = _StmtString->Get(_CurrentCharLocation + 1);

            _CurrentCharLocation++;

            // Check if we're starting a string.
            if (currentChar == '%' && _StmtString->PeekSearch(SYMPL_STRING_TOKEN, GetCurrentCharLocation() - 1)) {
//                stmtEntryStr->Append(SYMPL_STRING_TOKEN);
                SetCurrentCharLocation(GetCurrentCharLocation() + strlen(SYMPL_STRING_TOKEN) - 1);
                recording = !recording;
                continue;
            }

            // Save our current character.
            if (recording || (currentChar != '#' && currentChar != ';' &&
                              currentChar != '(' && currentChar != '{' &&
                              currentChar != ')')) {
                stmtEntryStr->AppendByte(currentChar);
            }

            // Check if we're in an array.
            if (!recording && currentChar == '[' && varObject->GetType() == ScriptObjectType::Array) {
                auto arrayResolver = SymplRegistry.Get<ArrayResolver>();
                stmtEntryStr->Append(
                        arrayResolver->Resolve(this, stmtEntryStr, varObject, currentOp).AsString());
                continue;
            }

            // Attempt to resolve value from an array that's being passed to a non-array (i.e. method: printl).
            if (!recording && nextChar == '[' && varObject->GetType() != ScriptObjectType::Array) {
                auto arrayResolver = SymplRegistry.Get<ArrayResolver>();
                stmtEntryStr->Append(
                        arrayResolver->ResolveValue(this, stmtEntryStr, varObject).AsString());
                continue;
            }

            // Check if we're in a parenth or method.
            if (!recording && currentChar == '(') {
                // Check to see if the varObject we have give is a method. If it's coming from
                // the Interpreter it may be a method and we don't want to skip it!
                if (stmtEntryStr->Empty() && varObject->GetType() == ScriptObjectType::Method) {
                    auto methodResolver = SymplRegistry.Get<MethodResolver>();
                    stmtEntryStr->Append(
                            methodResolver->Resolve(this, stmtEntryStr, varObject, currentOp).AsString());
                    statement_has_method = true;
                } else {
                    // Determine if the current statement buffer is an existing method.
                    ScriptObject* existingMethod = nullptr;

                    // Check if we're attempting to allocate or deallocate an object.
                    if (currentOp == StatementOperator::NewObject) {
                        std::string constructorPath = fmt::format("{0}.@{1}",
                                                                  stmtEntryStr->CStr(),
                                                                  stmtEntryStr->CStr()
                        );

                        existingMethod = ScriptVMInstance->FindObjectByPath(
                                constructorPath.c_str(),
                                nullptr
                        );
                        if (!existingMethod->IsEmpty()) {
                            sympl_assert(existingMethod->GetParent()->GetParent()->IsClass() && "Illegal call to 'new' on non-class object!");

                            varObject->SetIsClass(true);
                            varObject->CopyChildrenFrom(existingMethod->GetParent()->GetParent().Ptr());

                            // Call the constructor.
                            auto constructor = varObject->GetChildren()[0].Ptr()->FindChildByName(
                                    fmt::format("@{0}", varObject->GetName()).c_str()
                            );
                            if (!constructor->IsEmpty()) {
                                constructor->Evaluate();
                            }

                            stmtEntryStr->Clear();
                            continue;
                        } else {
                            // No constructor, so build the object without one.
                            std::string nonConstructorPath = fmt::format("{0}", stmtEntryStr->CStr());
                            existingMethod = ScriptVMInstance->FindObjectByPath(
                                    nonConstructorPath.c_str(),
                                    nullptr
                            );
                            sympl_assert(existingMethod->IsClass() && "Illegal call to 'new' on non-class object!");

                            varObject->SetIsClass(true);
                            varObject->CopyChildrenFrom(existingMethod);

                            stmtEntryStr->Clear();
                            continue;
                        }
                    } else if (currentOp == StatementOperator::DeleteObject) {
                        std::string destructorPath = fmt::format("{0}.~{0}",
                                                                 stmtEntryStr->CStr(),
                                                                 stmtEntryStr->CStr()
                        );

                        existingMethod = ScriptVMInstance->FindObjectByPath(destructorPath.c_str(), nullptr);
                    }

                    // Check if this method is from an object path.
                    if (IsNullObject(existingMethod) || existingMethod->IsEmpty()) {
                        if (stmtEntryStr->Contains('.')) {
                            existingMethod = ScriptVMInstance->FindObjectByPath(
                                    stmtEntryStr->CStr(),
                                    varObject->GetParent().IsValid() ? varObject->GetParent().Ptr() : nullptr
                            );
                        } else {
                            existingMethod = ScriptVMInstance->GetMethodRegistry()->FindMethod(stmtEntryStr->CStr());
                        }
                    }

                    // Resolve our method.
                    if (!existingMethod->IsEmpty() && existingMethod->GetType() == ScriptObjectType::Method) {
                        auto methodResolver = SymplRegistry.Get<MethodResolver>();
                        stmtEntryStr->Append(
                                methodResolver->Resolve(this, stmtEntryStr, existingMethod, currentOp).AsString());
                        statement_has_method = true;
                    } else {
                        auto parenthResolver = SymplRegistry.Get<ParenthResolver>();
                        stmtEntryStr->Append(
                                parenthResolver->Resolve(this, stmtEntryStr, varObject, currentOp).AsString());
                    }
                }
                continue;
            }

            // Check if we need to create the statement entry.
            if (!recording && (currentChar == '#' || currentChar == ';' ||
                               _TokenHelper->IsOperator(currentChar) || _TokenHelper->IsOperator(nextChar))) {

                // Skip if we don't have anything to check.
                if (stmtEntryStr->Empty()) {
                    continue;
                }

                // Check if we are attempting to allocate an object.
                if (stmtEntryStr->Equals("new")) {
                    currentOp = StatementOperator::NewObject;
                    stmtEntryStr->Clear();
                    continue;
                }

                // Check/save the operator.
                if (_TokenHelper->IsOperator(stmtEntryStr->CStr())) {
                    // Check if we're a double operator.
                    if (_TokenHelper->IsOperator(nextChar)) {
                        stmtEntryStr->AppendByte(nextChar);
                        _CurrentCharLocation++;
                    }

                    currentOp = _SymbolToOp(stmtEntryStr->CStr());
                    stmtEntryStr->Clear();
                    continue;
                }

                // Create our entry.
                auto stmtEntry = mem_alloc_object(StatementEntry);
                stmtEntry->Op = currentOp;
                stmtEntry->ConstantValue = _IsNumber(stmtEntryStr->CStr());

                // Check if the value is a boolean value.
                if (stmtEntry->ConstantValue.IsEmpty()) {
                    stmtEntry->ConstantValue = _IsBoolean(stmtEntryStr->CStr());
                }

                // Check to see if we're an object or string.
                if (stmtEntry->ConstantValue.IsEmpty()) {
                    ScriptObject* obj = nullptr;
                    if (varObject->GetType() == ScriptObjectType::Method) {
                        // Check if our string is structured like a path.
//                        if (stmtEntryStr->Contains('.')) {
//                            obj = ScriptVMInstance->FindObjectByPath(
//                                    stmtEntryStr->CStr(),
//                                    varObject->GetParent().IsValid() ? varObject->GetParent().Ptr() : nullptr
//                            );
//                        }

                        if (IsNullObject(obj) || obj->IsEmpty()) {
                            obj = to_method(varObject)->GetScope()->GetContext()->FindVariable(stmtEntryStr->CStr(),
                                                                                               true);
                            // Check if we're an object.
                            if (obj->IsEmpty()) {
                                obj = to_method(varObject)->GetScope()->GetContext()->FindObject(stmtEntryStr->CStr(),
                                                                                                 true);
                            }
                            // Check if we're a method.
                            if (obj->IsEmpty()) {
                                obj = ScriptVMInstance->GetMethodRegistry()->FindMethod(stmtEntryStr->CStr());
                            }
                        }
                    } else {
//                        if (stmtEntryStr->Contains('.')) {
//                            obj = ScriptVMInstance->FindObjectByPath(
//                                    stmtEntryStr->CStr(),
//                                    varObject->GetParent().IsValid() ? varObject->GetParent().Ptr() : nullptr
//                            );
//                        } else {
                            obj = varObject->GetContext()->FindVariable(stmtEntryStr->CStr(), true);
//                        }
                    }

                    // If this is an empty then we will consider it as a string.
                    if (obj->IsEmpty()) {
                        stmtEntry->ConstantValue = stmtEntryStr->CStr();
                    } else {
                        stmtEntry->ObjectValue = obj;

                        // Update our statement type if this object is a method.
                        if (obj->GetType() == ScriptObjectType::Method && !to_method(obj)->IsImmediate()) {
                            auto retType = to_method(obj)->GetReturnType();
                            switch ((int) retType) {
                                case (int) MethodReturnType::Object:
                                    SetType(StatementType::Object);
                                    break;
                                case (int) MethodReturnType::String:
                                    SetType(StatementType::String);
                                    break;
                                case (int) MethodReturnType::Int:
                                    SetType(StatementType::Integer);
                                    break;
                                case (int) MethodReturnType::Float:
                                    SetType(StatementType::Float);
                                    break;
                                case (int) MethodReturnType::Bool:
                                    SetType(StatementType::Bool);
                                    break;
                            }
                        }
                    }
                }

                _StmtEntries.push_back(stmtEntry);
                stmtEntryStr->Clear();
                continue;
            }
        }

        mem_free_ref(StringBuffer, stmtEntryStr);
    }

    // Cache if we haven't cached already.
    if (!cache && !statement_has_method && IsNullObject(stmtCache) && !IsNullObject(cacheContext)) {
        stmtCache = cacheContext->GetStatementCache()->Cache(
                str.c_str(),
                _StmtEntries
        );
    }

    // Resolve the statements and update the value.
    Variant retVal;
    if (!IsNullObject(stmtCache) && !statement_has_method) {
        retVal = _ResolveStatements(stmtCache->Entries);
    } else {
        retVal = _ResolveStatements(_StmtEntries);
    }

    if (!cache) {
        ClearStatementEntries();
    }

    _InUse = false;
    return retVal;
}

Variant StatementResolver::_ResolveStatements(const std::vector<StatementEntry*>& stmtEntries)
{
    if (stmtEntries.empty()) {
        return Variant::Empty;
    }

    // Evaluate if we only have 1 statement entry.
    if (stmtEntries.size() == 1) {
        auto entry = stmtEntries[0];
        if (entry->Op == StatementOperator::Equals) {
            if (entry->ObjectValue.IsValid()) {
                if (entry->ObjectValue->GetType() == ScriptObjectType::Object ||
                    entry->ObjectValue->GetType() == ScriptObjectType::Method) {
                    return entry->ObjectValue.Ptr();
                }
                return entry->ObjectValue->GetValue();
            }
            return entry->ConstantValue;
        }
    }

    // Set the type.
    if (_Type == StatementType::None) {
        Variant firstVal = stmtEntries[0]->ObjectValue.IsValid() ?
                           stmtEntries[0]->ObjectValue->GetValue() : stmtEntries[0]->ConstantValue;

        auto type = _FindType(firstVal);
        _Type = type;
    }

    Variant value;
    auto size = stmtEntries.size();
    for (unsigned i = 0; i < size; i++) {
        _Solve(stmtEntries[i], value);
    }

    return value;
}

void StatementResolver::_Solve(StatementEntry* entry, Variant& value)
{
    Variant evalValue;
    if (entry->ObjectValue.IsValid()) {
        evalValue = entry->ObjectValue->GetValue();
    } else {
        // We're dealing with a constant value.
        evalValue = entry->ConstantValue;
    }

    // Check to see if we are just assigning an empty value.
    if (value.GetType() == VariantType::Empty) {
        value = evalValue;
        return;
    }

    switch ((int)entry->Op) {
        case (int)StatementOperator::Add:
            if (_Type == StatementType::String) {
                value.GetStringBuffer()->Append(evalValue.GetStringBuffer()->CStr());
            } else if (_Type == StatementType::Integer) {
                value.Set(value.GetInt() + evalValue.GetInt());
            }
            break;
        case (int)StatementOperator::Subtract:
            if (_Type == StatementType::Integer) {
                value.Set(value.GetInt() - evalValue.GetInt());
            } else if (_Type == StatementType::Float) {
                value.Set(value.GetFloat() - evalValue.GetFloat());
            }
            break;
        case (int)StatementOperator::Divide:
            if (_Type == StatementType::Integer) {
                value.Set(value.GetInt() / evalValue.GetInt());
            } else if (_Type == StatementType::Float) {
                value.Set(value.GetFloat() / evalValue.GetFloat());
            }
            break;
        case (int)StatementOperator::Multiply:
            if (_Type == StatementType::Integer) {
                value.Set(value.GetInt() * evalValue.GetInt());
            } else if (_Type == StatementType::Float) {
                value.Set(value.GetFloat() * evalValue.GetFloat());
            }
            break;
        case (int)StatementOperator::Mod:
            if (_Type == StatementType::Integer) {
                value.Set(value.GetInt() % evalValue.GetInt());
            }
            break;
        case (int)StatementOperator::GreaterThan:
            if (_Type == StatementType::Integer) {
                value.Set(value.GetInt() > evalValue.GetInt());
            } else if (_Type == StatementType::Float) {
                value.Set(value.GetFloat() > evalValue.GetFloat());
            }
            break;
        case (int)StatementOperator::LessThan:
            if (_Type == StatementType::Integer) {
                value.Set(value.GetInt() < evalValue.GetInt());
            } else if (_Type == StatementType::Float) {
                value.Set(value.GetFloat() < evalValue.GetFloat());
            }
            break;
        case (int)StatementOperator::GreaterEqualThan:
            if (_Type == StatementType::Integer) {
                value.Set(value.GetInt() >= evalValue.GetInt());
            } else if (_Type == StatementType::Float) {
                value.Set(value.GetFloat() >= evalValue.GetFloat());
            }
            break;
        case (int)StatementOperator::LessEqualThan:
            if (_Type == StatementType::Integer) {
                value.Set(value.GetInt() <= evalValue.GetInt());
            } else if (_Type == StatementType::Float) {
                value.Set(value.GetFloat() <= evalValue.GetFloat());
            }
            break;
        case (int)StatementOperator::IsEqual2:
            if (_Type == StatementType::Integer) {
                value.Set(value.GetInt() == evalValue.GetInt());
            } else if (_Type == StatementType::Float) {
                value.Set(value.GetFloat() == evalValue.GetFloat());
            } else if (_Type == StatementType::String) {
                value.Set(value.GetStringBuffer()->Equals(evalValue.GetStringBuffer()));
            }
            break;
        case (int)StatementOperator::NotIsEqual2:
            if (_Type == StatementType::Integer) {
                value.Set(value.GetInt() != evalValue.GetInt());
            } else if (_Type == StatementType::Float) {
                value.Set(value.GetFloat() != evalValue.GetFloat());
            } else if (_Type == StatementType::String) {
                value.Set(!value.GetStringBuffer()->Equals(evalValue.GetStringBuffer()));
            }
            break;
        default:
            value.Set(evalValue);
            break;
    }
}

Variant StatementResolver::_IsNumber(const char* str)
{
    char firstChar = *str;
    if (firstChar == '0' || firstChar == '1' || firstChar == '2' ||
        firstChar == '3' || firstChar == '4' || firstChar == '5' ||
        firstChar == '6' || firstChar == '7' || firstChar == '8' ||
        firstChar == '9') {

        long long intVal;
        float floatVal;

        if (NumberHelper::TryParseLong(str, &intVal)) {
            return intVal;
        } else if (NumberHelper::TryParseFloat(str, &floatVal)) {
            return floatVal;
        }

    }
    return Variant::Empty;
}

Variant StatementResolver::_IsBoolean(const char* str)
{
    if (strcmp(str, "true") == 0 || strcmp(str, "false") == 0) {
        _Type = StatementType::Bool;
        return strcmp(str, "true") == 0 ? true : false;
    }
    return Variant::Empty;
}

StatementType StatementResolver::_FindType(const Variant& value)
{
    if (value.GetType() == VariantType::Bool) {
        return StatementType::Bool;
    }
    if (value.GetType() == VariantType::Int) {
        return StatementType::Integer;
    }
    if (value.GetType() == VariantType::Float) {
        return StatementType::Float;
    }
    if (value.GetType() == VariantType::StringBuffer) {
        return StatementType::String;
    }

    return StatementType::Object;
}

StatementOperator StatementResolver::_SymbolToOp(const char* symbol)
{
    if (strcmp(symbol, "=") == 0) {
        return StatementOperator::Equals;
    }
    if (strcmp(symbol, "+") == 0) {
        return StatementOperator::Add;
    }
    if (strcmp(symbol, "-") == 0) {
        return StatementOperator::Subtract;
    }
    if (strcmp(symbol, "/") == 0) {
        return StatementOperator::Divide;
    }
    if (strcmp(symbol, "*") == 0) {
        return StatementOperator::Multiply;
    }
    if (strcmp(symbol, ">") == 0) {
        return StatementOperator::GreaterThan;
    }
    if (strcmp(symbol, "<") == 0) {
        return StatementOperator::LessThan;
    }
    if (strcmp(symbol, ">=") == 0) {
        return StatementOperator::GreaterEqualThan;
    }
    if (strcmp(symbol, "<=") == 0) {
        return StatementOperator::LessEqualThan;
    }
    if (strcmp(symbol, "==") == 0) {
        return StatementOperator::IsEqual2;
    }
    if (strcmp(symbol, "!=") == 0) {
        return StatementOperator::NotIsEqual2;
    }

    return StatementOperator::None;
}

void StatementResolver::ClearStatementEntries()
{
    for (auto stmtEntry : _StmtEntries) {
        mem_free_object(StatementEntry, stmtEntry);
    }
    _StmtEntries.clear();
}

bool StatementResolver::Release()
{
    mem_free_ref(StringBuffer, _StmtString);
    ClearStatementEntries();
    return true;
}
