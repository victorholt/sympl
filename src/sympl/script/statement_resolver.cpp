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
#include <sympl/util/number_helper.h>
#include <sympl/util/string_helper.h>
sympl_namespaces

Variant EvalResolver::GetEvalFromStatementBuffer(const char* stmtStr, ScriptObject* scriptObject)
{
    assert((strlen(stmtStr) > 0) && "Attempting to evaluate an invalid statement!");

    // Create the statement and set the string.
    auto stmtResolver = SymplRegistry.Get<StatementResolver>();
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
    if (varObject->GetType() != ScriptObjectType::Method) {
        scriptObject = ScriptVMInstance->GetMethodRegistry()->FindMethod(currentStr->CStr());
    }

    // Ensure this is a proper method to resolve.
    if (IsNullObject(scriptObject) || scriptObject->IsEmpty() || scriptObject->GetType() != ScriptObjectType::Method) {
        return Variant::Empty;
    }

    // Remove the name of the method so it's not part of the statement value.
    currentStr->Replace(scriptObject->GetCleanName().c_str(), "");

    // Check if we have arguments cached.

    MethodArgCache* methodArgCache = to_method(scriptObject)->FindOrCreateArgCache(stmtResolver->GetStatementString()->CStr());
    if (!IsNullObject(methodArgCache) && methodArgCache->Args.Size() == to_method(scriptObject)->GetNumArgs()) {
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
    SharedPtr<StringBuffer> resolveStr = mem_alloc_ref(StringBuffer);

    // Current/last concatenating condition.
    std::string currentConcatConditionStr;
    std::string lastConcatConditionStr;

    // Current argument value.
    Variant argValue;

    // Build out the argument statements.
    while (stmtResolver->GetCurrentCharLocation() < statementStr->Length()) {
        currentChar = statementStr->Get(stmtResolver->GetCurrentCharLocation());
        nextChar = statementStr->Get(stmtResolver->GetCurrentCharLocation() + 1);
        stmtResolver->SetCurrentCharLocation(stmtResolver->GetCurrentCharLocation() + 1);

        if (currentChar == '"') {
            resolveStr->Append(SYMPL_STRING_TOKEN);
            recording = !recording;
        }

        if (!recording && currentChar == '(') {
            // Check to see if the varObject we have give is a method. If it's coming from
            // the Interpreter it may be a method and we don't want to skip it!
            if (resolveStr->Empty() && varObject->GetType() == ScriptObjectType::Method) {
                auto methodResolver = mem_alloc_object(MethodResolver);
                resolveStr->Append(methodResolver->Resolve(stmtResolver, resolveStr.Ptr(), varObject, op).AsString());
                mem_free_object(MethodResolver, methodResolver);
            } else {
                // Determine if the current statement buffer is an existing method.
                auto existingMethod = ScriptVMInstance->GetMethodRegistry()->FindMethod(resolveStr->CStr());
                if (!existingMethod->IsEmpty() && existingMethod->GetType() == ScriptObjectType::Method) {
                    auto methodResolver = mem_alloc_object(MethodResolver);
                    resolveStr->Append(methodResolver->Resolve(stmtResolver, resolveStr.Ptr(), varObject, op).AsString());
                    mem_free_object(MethodResolver, methodResolver);
                } else {
                    auto parenthResolver = mem_alloc_object(ParenthResolver);
                    resolveStr->Append(parenthResolver->Resolve(stmtResolver, resolveStr.Ptr(), existingMethod, op).AsString());
                    mem_free_object(ParenthResolver, parenthResolver);
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
                    obj = to_method(varObject)->GetScope()->GetContext()->FindVariable(resolveStr->CStr(), true);
                } else {
                    obj = varObject->GetContext()->FindVariable(resolveStr->CStr(), true);
                }
                if (!obj->IsEmpty()) {
                    objectValue = obj->GetValue();
                }
            }

            // Check if we were able to find an object value.
            MethodArgCacheValue cacheValue;
            cacheValue.ArgStringLength = resolveStr->Length();

            if (!objectValue.IsEmpty()) {
                args.Push(objectValue);

                if (!IsNullObject(methodArgCache)) {
                    if (!IsNullObject(obj)) {
                        cacheValue.ArgValue = obj;
                        methodArgCache->Args.Push(cacheValue);
                    } else {
                        cacheValue.ArgValue = objectValue;
                        methodArgCache->Args.Push(cacheValue);
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
                    methodArgCache->Args.Push(cacheValue);
                }
            }

            argValue = Variant::Empty;
            currentConcatConditionStr = "";
            lastConcatConditionStr = "";
            resolveStr->Clear();

            // We've completed building out the arguments.
            if (currentChar == ')') {
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
    assert(false && "Unclosed call to method!");
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
    SharedPtr<StringBuffer> resolveStr = mem_alloc_ref(StringBuffer);

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

        if (currentChar == '"') {
            resolveStr->Append(SYMPL_STRING_TOKEN);
            recording = !recording;
        }

        // Resolve another contained value.
        if (!recording && currentChar == '(') {
            // Determine if the current statement buffer is an existing method.
            auto existingMethod = ScriptVMInstance->GetMethodRegistry()->FindMethod(resolveStr->CStr());
            if (!existingMethod->IsEmpty() && existingMethod->GetType() == ScriptObjectType::Method) {
                auto methodResolver = mem_alloc_object(MethodResolver);
                resolveStr->Append(methodResolver->Resolve(stmtResolver, resolveStr.Ptr(), existingMethod, op).AsString());
                mem_free_object(MethodResolver, methodResolver);
            } else {
                auto parenthResolver = mem_alloc_object(ParenthResolver);
                resolveStr->Append(parenthResolver->Resolve(stmtResolver, resolveStr.Ptr(), existingMethod, op).AsString());
                mem_free_object(ParenthResolver, parenthResolver);
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
            return retValue;
        }

        if (recording || (!recording && currentChar != '#')) {
            resolveStr->AppendByte(currentChar);
        }
    }

    // We failed to exit the while loop early!
    assert(false && "Unclosed parenthesis in statement found!");
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

Variant StatementResolver::Resolve(const char* str, ScriptObject* varObject, bool cache)
{
    // Clear out the previous statement and append the new statement.
    if (_StmtEntries.empty()) {
        _StmtString->Clear();
        _StmtString->Append(str);
        if (_StmtString->LastByte() != ';') {
            _StmtString->AppendByte(';');
        }

        // Set our default operator.
        StatementOperator currentOp = StatementOperator::Equals;

        // Go through our string and create our entries.
        SharedPtr<StringBuffer> stmtEntryStr = mem_alloc_ref(StringBuffer);
        char currentChar = '\0';
        char nextChar = '\0';
        bool recording = false;
        _CurrentCharLocation = 0;

        while (_CurrentCharLocation < _StmtString->Length()) {
            currentChar = _StmtString->Get(_CurrentCharLocation);
            nextChar = _StmtString->Get(_CurrentCharLocation + 1);
            _CurrentCharLocation++;

            // Check if we're starting a string.
            if (currentChar == '"') {
                stmtEntryStr->Append(SYMPL_STRING_TOKEN);
                recording = !recording;
                continue;
            }

            // Save our current character.
            if (recording || (currentChar != '#' && currentChar != ';' &&
                              currentChar != '(' && currentChar != '{')) {
                stmtEntryStr->Append(std::string(1, currentChar));
            }

            // Check if we're in a parenth or method.
            if (!recording && currentChar == '(') {
                // Check to see if the varObject we have give is a method. If it's coming from
                // the Interpreter it may be a method and we don't want to skip it!
                if (stmtEntryStr->Empty() && varObject->GetType() == ScriptObjectType::Method) {
                    auto methodResolver = mem_alloc_object(MethodResolver);
                    stmtEntryStr->Append(
                            methodResolver->Resolve(this, stmtEntryStr.Ptr(), varObject, currentOp).AsString());
                    mem_free_object(MethodResolver, methodResolver);
                } else {
                    // Determine if the current statement buffer is an existing method.
                    auto existingMethod = ScriptVMInstance->GetMethodRegistry()->FindMethod(stmtEntryStr->CStr());
                    if (!existingMethod->IsEmpty() && existingMethod->GetType() == ScriptObjectType::Method) {
                        auto methodResolver = mem_alloc_object(MethodResolver);
                        stmtEntryStr->Append(
                                methodResolver->Resolve(this, stmtEntryStr.Ptr(), varObject, currentOp).AsString());
                        mem_free_object(MethodResolver, methodResolver);
                    } else {
                        auto parenthResolver = mem_alloc_object(ParenthResolver);
                        stmtEntryStr->Append(
                                parenthResolver->Resolve(this, stmtEntryStr.Ptr(), varObject, currentOp).AsString());
                        mem_free_object(ParenthResolver, parenthResolver);
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
                auto stmtEntry = alloc_bytes(StatementEntry);
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
                        obj = to_method(varObject)->GetScope()->GetContext()->FindVariable(stmtEntryStr->CStr(), true);
                    } else {
                        obj = varObject->GetContext()->FindVariable(stmtEntryStr->CStr(), true);
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
    }

    // Resolve the statements and update the value.
    auto retVal = _ResolveStatements(_StmtEntries);

    if (!cache) {
        ClearStatementEntries();
    }
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
            return (entry->ObjectValue.IsValid() ? entry->ObjectValue->GetValue() : entry->ConstantValue);
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
    for (unsigned i = 0; i < stmtEntries.size(); i++) {
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
    }
}

Variant StatementResolver::_IsNumber(const char* str)
{
    long long intVal;
    float floatVal;

    if (NumberHelper::TryParseLong(str, &intVal)) {
        return intVal;
    } else if (NumberHelper::TryParseFloat(str, &floatVal)) {
        return floatVal;
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
        delete stmtEntry;
    }
    _StmtEntries.clear();
}

bool StatementResolver::Release()
{
    mem_free_ref(StringBuffer, _StmtString);
    ClearStatementEntries();
}
