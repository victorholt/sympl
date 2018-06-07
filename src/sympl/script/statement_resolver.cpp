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

class EvalResolver
{
public:
    //! Constructor.
    EvalResolver() = default;

    //! Returns a value from evaluating a given script object.
    //! \param stmtStr
    //! \param scriptObject
    //! \return
    Variant GetEvalFromStatementBuffer(const char* stmtStr, ScriptObject* scriptObject)
    {
        assert((strlen(stmtStr) > 0) && "Attempting to evaluate an invalid statement!");

        // Create the statement and set the string.
        SharedPtr<StatementResolver> stmtResolver = alloc_ref(StatementResolver);
        return stmtResolver->Resolve(stmtStr, scriptObject);
    }

    //! Returns whether or not a given script object evaluates to true.
    //! \param stmtStr
    //! \param scriptObject
    //! \return
    bool IsStatementBufferTrue(const char* stmtStr, ScriptObject* scriptObject)
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
};

class MethodResolver
{
public:
    //! Constructor.
    MethodResolver() = default;

    //! Attempts to resolve a method from a given string.
    Variant Resolve(StatementResolver* stmtResolver, StringBuffer* currentStr,
                    ScriptObject* varObject, StatementOperator op)
    {
        auto tokenHelper = ScriptVMInstance->GetScriptToken();
        auto statementStr = stmtResolver->GetStatementString();

//        if (strlen(currentStr) != 0) {
//            stmtResolver->SetCurrentCharLocation(stmtResolver->GetCurrentCharLocation() - 1);
//        }

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

        // Parse out the object value.
        char currentChar = '\0';
        char nextChar = '\0';
        bool recording = false;

        // String to resolve.
        SharedPtr<StringBuffer> resolveStr = alloc_ref(StringBuffer);

        // Current/last concatenating condition.
        std::string currentConcatConditionStr;
        std::string lastConcatConditionStr;

        // Save args in the variant.
        Urho3D::PODVector<Variant> args;

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
                    MethodResolver methodResolver;
                    resolveStr->Append(methodResolver.Resolve(stmtResolver, resolveStr.Ptr(), varObject, op).AsString());
                } else {
                    // Determine if the current statement buffer is an existing method.
                    auto existingMethod = ScriptVMInstance->GetMethodRegistry()->FindMethod(resolveStr->CStr());
                    if (!existingMethod->IsEmpty() && existingMethod->GetType() == ScriptObjectType::Method) {
                        MethodResolver methodResolver;
                        resolveStr->Append(methodResolver.Resolve(stmtResolver, resolveStr.Ptr(), varObject, op).AsString());
                    } else {
//                        _StatementBuffer->Append(_ResolveParenth(varObject, statementStr, op).AsString());
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
                    if (lastConcatConditionStr.size() == 0) {
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
};

StatementResolver::StatementResolver()
{
    __Construct();
}

void StatementResolver::__Construct()
{
    _StmtString = alloc_ref(StringBuffer);
    _TokenHelper = ScriptVMInstance->GetScriptToken();
}

Variant StatementResolver::Resolve(const char* str, ScriptObject* varObject)
{
    // Clear out the previous statement and append the new statement.
    _StmtString->Clear();
    _StmtString->Append(str);
    if (_StmtString->LastByte() != ';') {
        _StmtString->AppendByte(';');
    }
    _ClearStatements();

    // Set our default operator.
    StatementOperator currentOp = StatementOperator::Equals;

    // Go through our string and create our entries.
    SharedPtr<StringBuffer> stmtEntryStr = alloc_ref(StringBuffer);
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
                MethodResolver mResolver;
                stmtEntryStr->Append(mResolver.Resolve(this, stmtEntryStr.Ptr(), varObject, currentOp).AsString());
            } else {
                // Determine if the current statement buffer is an existing method.
                auto existingMethod = ScriptVMInstance->GetMethodRegistry()->FindMethod(stmtEntryStr->CStr());
                if (!existingMethod->IsEmpty() && existingMethod->GetType() == ScriptObjectType::Method) {
                    MethodResolver methodResolver;
                    stmtEntryStr->Append(methodResolver.Resolve(this, stmtEntryStr.Ptr(), varObject, currentOp).AsString());
                } else {
                    stmtEntryStr->Append(_ResolveParenth(varObject, currentOp).AsString());
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

            _Statements.Push(stmtEntry);
            stmtEntryStr->Clear();
            continue;
        }
    }

    // Resolve the statements and update the value.
    return _ResolveStatements();
}

Variant StatementResolver::_ResolveStatements()
{
    if (_Statements.Empty()) {
        return Variant::Empty;
    }

    // Evaluate if we only have 1 statement entry.
    if (_Statements.Size() == 1) {
        auto entry = _Statements[0];
        if (entry->Op == StatementOperator::Equals) {
            return (entry->ObjectValue.IsValid() ? entry->ObjectValue->GetValue() : entry->ConstantValue);
        }
    }

    // Set the type.
    if (_Type == StatementType::None) {
        Variant firstVal = _Statements[0]->ObjectValue.IsValid() ?
                _Statements[0]->ObjectValue->GetValue() : _Statements[0]->ConstantValue;

        auto type = _FindType(firstVal);
        _Type = type;
    }

    Variant value;
    for (auto entry : _Statements) {
//        Variant entryValue = entry->ObjectValue.IsValid() ?
//                           entry->ObjectValue->GetValue() : entry->ConstantValue;

//        auto type = _FindType(entryValue);
//        if (type != _Type && _Type != StatementType::String) {
//            if (type == StatementType::String) {
//                _Type = StatementType::String;
//            } else {
//                assert(false && "Attempted to assign two different types in statement");
//            }
//        }

        _Solve(entry, value);
    }

    return value;
}

Variant StatementResolver::_ResolveParenth(ScriptObject* varObject, StatementOperator op)
{
    std::cout << "Resolving parenth" << std::endl;
    return Variant::Empty;
}

Variant StatementResolver::_ResolveMethod(ScriptObject* varObject, StatementOperator op)
{
    std::cout << "Resolving method" << std::endl;
    return Variant::Empty;
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

void StatementResolver::_ClearStatements()
{
    for (auto entry : _Statements) {
        free_bytes(entry);
    }
    _Statements.Clear();
}

bool StatementResolver::Release()
{
    free_ref(_StmtString);
    _ClearStatements();
}
