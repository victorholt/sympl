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
#include <sympl/script/cache/script_cache.h>
#include <sympl/script/script_vm.h>
#include <sympl/script/resolvers/array_resolver.h>
#include <sympl/script/resolvers/method_resolver.h>
//#include <sympl/script/resolvers/parenth_resolver.h>
#include <sympl/util/string_helper.h>
#include <sympl/util/number_helper.h>

sympl_namespaces

StatementResolver::StatementResolver()
{
    __Construct();
}

void StatementResolver::__Construct()
{
    _TokenHelper = SymplRegistry.Get<ScriptToken>();
}

Variant StatementResolver::Resolve(const std::string& stmtStr, ScriptObject* destObject)
{
    if (!_StmtString.IsValid()) {
        _StmtString = mem_alloc_ref(StringBuffer);
    } else {
        _StmtString->Clear();
    }
    _StmtString->Append(stmtStr);

    if (_StmtString->LastByte() != ';') {
        _StmtString->AppendByte(';');
    }

    auto stmtEntryStr = mem_alloc_ref(StringBuffer);
    char currentChar = '\0';
    char nextChar = '\0';

    bool recording = false;
    bool isString = false;

    StatementOperator currentOp = StatementOperator::Equals;

    ScriptCacheObject* cacheObject = nullptr;
    if (cache) {
        cacheObject = ScriptCacheInstance.Fetch(destObject);
        if (cacheObject->HasKey(stmtStr)) {

        }
    }

    while (_CharLocation < _StmtString->Length()) {
        currentChar = _StmtString->Get(_CharLocation);
        nextChar = _StmtString->Get(_CharLocation + 1);
        _CharLocation++;

        // Check if we're starting a string.
        if (currentChar == '%' && _StmtString->PeekSearch(SYMPL_STRING_TOKEN, _CharLocation - 1)) {
            stmtEntryStr->Append(SYMPL_STRING_TOKEN); // We need to append the token so that we don't look for
            // a class with the same name on object lookup.
            SetCharLocation(_CharLocation + strlen(SYMPL_STRING_TOKEN) - 1);
            recording = !recording;
            isString = true;
            _Type = StatementType::String;
            continue;
        }

        // Save our current character.
        if (recording || (currentChar != '#' && currentChar != ';' &&
                          currentChar != '(' && currentChar != '{' &&
                          currentChar != ')')) {
            stmtEntryStr->AppendByte(currentChar);
        }

        // Check if we're in an array.
        if (!recording && currentChar == '[') {
            auto arrayResolver = SymplRegistry.Get<ArrayResolver>();
            stmtEntryStr->Append(
                    arrayResolver->Resolve(this, stmtEntryStr, destObject, currentOp).AsString());
            continue;
        }

        // Check if we're in a parenth or method.
        if (!recording && currentChar == '(') {
            if (stmtEntryStr->Empty() && destObject->GetType() == ScriptObjectType::Method) {
                // Resolve our method.
                auto methodResolver = SymplRegistry.Get<MethodResolver>();
                stmtEntryStr->Append(
                        methodResolver->Resolve(this, stmtEntryStr, destObject, currentOp).AsString());
            } else {
                auto existingMethod = ScriptVMInstance.FindObjectByPath(stmtEntryStr->CStr());
                sympl_assert(!stmtEntryStr->Empty() && existingMethod->IsMethod(), "Invalid method called!");

                if (!existingMethod->IsEmpty() && existingMethod->GetType() == ScriptObjectType::Method) {
                    auto methodResolver = SymplRegistry.Get<MethodResolver>();
                    stmtEntryStr->Append(
                            methodResolver->Resolve(this, stmtEntryStr, existingMethod, currentOp).AsString());
                } else {
                    sympl_assert(false, "Parenth Resolver not yet implemented!");
                    //                auto parenthResolver = SymplRegistry.Get<ParenthResolver>();
                    //                stmtEntryStr->Append(
                    //                        parenthResolver->Resolve(this, stmtEntryStr, varObject, currentOp).AsString());
                }
            }
        }

        // Skip processing if we're current recording.
        if (recording) { continue; }

        // Check if we need to create the statement entry.
        if (currentChar == '#' || currentChar == ';' ||
           _TokenHelper->IsOperator(currentChar) || _TokenHelper->IsOperator(nextChar)) {

            // Skip if we don't have anything to check.
            if (stmtEntryStr->Empty()) {
                continue;
            }

            // Check/save the operator.
            if (stmtEntryStr->Length() <= 2 && _TokenHelper->IsOperator(stmtEntryStr->CStr())) {
                // Check if we're a double operator.
                if (_TokenHelper->IsOperator(nextChar)) {
                    stmtEntryStr->AppendByte(nextChar);
                    _CharLocation++;
                }

                currentOp = _SymbolToOp(stmtEntryStr->CStr());
                stmtEntryStr->Clear();
                continue;
            }

            // Create our entry.
            auto stmtEntry = mem_alloc_object(StatementEntry);
            stmtEntry->Op = currentOp;

            // Handle case if this is a string.
            if (isString || stmtEntryStr->StartsWith(SYMPL_STRING_TOKEN)) {
                _Type = StatementType::String;

                // Constants will be decoded.
                stmtEntryStr->Replace(SYMPL_STRING_TOKEN, "");

                std::string decodedStr;
                if (_TokenHelper->DecodeSpecialCharString(stmtEntryStr->CStr(), decodedStr)) {
                    stmtEntryStr->Clear();
                    stmtEntryStr->Append(decodedStr);
                }

                stmtEntry->ConstantValue = stmtEntryStr->CStr();
            } else {
                NumberHelper::IsNumber(stmtEntryStr->CStr(), stmtEntry->ConstantValue);

                // Check if the value is a boolean value.
                if (stmtEntry->ConstantValue.IsEmpty()) {
                    stmtEntry->ConstantValue = _IsBoolean(stmtEntryStr->CStr());
                }
            }

            // Handle case if this is an object.
            if (!isString && stmtEntry->ConstantValue.IsEmpty()) {
                // Attempt to find the object we're looking for.
                ScriptObject* scopeObj = &ScriptObject::Empty;

                if (!destObject->IsMethod()) {
                    scopeObj = destObject->FindChildByName(stmtEntryStr->CStr());
                } else {
                    scopeObj = to_method(destObject)->GetScope()->FindChildByName(stmtEntryStr->CStr());
                }
                sympl_assert(!scopeObj->IsEmpty(), "Illegal use of non-declared object!");

                stmtEntry->ObjectValue = scopeObj;

                // If we cannot discover the object treat it as a string.
//                if (scopeObj->IsEmpty()) {
//                    _Type = StatementType::String;
//                    stmtEntry->ConstantValue = stmtEntryStr->CStr();
//                } else {
//                    stmtEntry->ObjectValue = scopeObj;
//                }
            }

            // Save our entry.
            _StmtEntries.emplace_back(stmtEntry);
            stmtEntryStr->Clear();
            isString = false;
        }
    }

    mem_free_ref(StringBuffer, stmtEntryStr);

    // Resolve the statements and update the value.
    Variant retVal;
    retVal = _ResolveStatements(_StmtEntries);
    ClearStatementEntries();
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
        if (entry->Op == StatementOperator::Equals || entry->Op == StatementOperator::NewObject) {
            if (entry->ObjectValue.IsValid()) {
                if (entry->ObjectValue->GetType() == ScriptObjectType::Object ||
                    entry->ObjectValue->GetType() == ScriptObjectType::Method) {
                    return entry->ObjectValue.Ptr();
                }

                // If we're a variable with no value, than we're just returning
                // the object.
                if (entry->ObjectValue->GetValue().IsEmpty()) {
                    return entry->ObjectValue.Ptr();
                } else {
                    return entry->ObjectValue->GetValue();
                }
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
        // Find the object's value.
        evalValue = entry->ObjectValue->GetValue();
        while (true) {
            if (evalValue.GetType() != VariantType::Object) {
                break;
            }
            evalValue = to_script_object(evalValue)->GetValue();
        }
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
                value.GetStringBuffer()->Append(evalValue.AsString());
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

Variant StatementResolver::_IsBoolean(const char* str)
{
    if (strcmp(str, "true") == 0 || strcmp(str, "false") == 0) {
        _Type = StatementType::Bool;
        return strcmp(str, "true") == 0;
    }
    return Variant::Empty;
}

StatementType StatementResolver::_FindType(const Variant& value)
{
    auto type = value.GetType();

    // If this is an object, figure out what the object
    // value is to discover the type.
    if (type == VariantType::Object) {
        Variant tmpValue = value;
        while (true) {
            if (tmpValue.GetType() != VariantType::Object) {
                break;
            }
            tmpValue = to_script_object(tmpValue)->GetValue();
        }
        type = tmpValue.GetType();
    }

    // Determine the type.
    if (type == VariantType::Bool) {
        return StatementType::Bool;
    }
    if (type == VariantType::Int) {
        return StatementType::Integer;
    }
    if (type == VariantType::Float) {
        return StatementType::Float;
    }
    if (type == VariantType::StringBuffer) {
        return StatementType::String;
    }

    return StatementType::Object;
}

void StatementResolver::ClearStatementEntries()
{
    for (auto stmtEntry : _StmtEntries) {
        mem_free_object(StatementEntry, stmtEntry);
    }
    _StmtEntries.clear();
    _CharLocation = 0;
    _Type = StatementType::None;

    if (_StmtString.IsValid()) {
        _StmtString->Clear();
    }
}

bool StatementResolver::Release()
{
    ClearStatementEntries();
    return true;
}
