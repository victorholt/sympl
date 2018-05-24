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
#include <sympl/script/script_statement.h>
#include <sympl/script/sympl_vm.h>
#include <sympl/script/methods/method_registry.h>
#include <sympl/core/sympl_number_helper.h>
sympl_namespaces

ScriptStatement::ScriptStatement() {
    _Type = StatementType::None;
    _String = alloc_ref(StringBuffer);
    _String->Resize(2000);

    _StatementBuffer = alloc_ref(StringBuffer);
    _StatementBuffer->Resize(2000);
}

ScriptStatement::~ScriptStatement() {
    for (auto entryIt : _Entries) {
        entryIt->ConstantValue.Clear();
        delete entryIt;
    }
    _Entries.clear();

    free_ref(StringBuffer, _StatementBuffer);
}

void ScriptStatement::Build(ScriptObject* varObject, StringBuffer* statementStr)
{
    assert(!IsNullObject(varObject) && !varObject->IsEmpty() && "Failed to evaluate statement on an unknown variable");

    if (IsNullObject(statementStr)) {
        statementStr = _String.Ptr();
    }

    StatementOperator currentOp = StatementOperator::Equals;
    bool recording = false;
    bool isString = false;
    _CurrentCharLocation = 0;

    if (statementStr->LastByte() != ';') {
        statementStr->AppendByte(';');
    }

    while (_CurrentCharLocation < statementStr->Length()) {
        char currentChar = statementStr->Get(_CurrentCharLocation);
        char nextChar = statementStr->Get(_CurrentCharLocation + 1);
        _CurrentCharLocation++;

        // Check if we need to record a string.
        if (currentChar == '"') {
            isString = true;
            recording = !recording;
            continue;
        }

        // Save our current character.
        if (recording || (!recording && currentChar != '#' && currentChar != ';' && currentChar != '(' && currentChar != '{')) {
            _StatementBuffer->AppendByte(currentChar);
        }

        // Determine if we're calling a method and attempt to resolve it.
        // Build a statement from the string within the parameters
        if (!recording && currentChar == '(') {
            // Determine if the current statement buffer is an existing method.
            auto existingMethod = SymplVMInstance->GetMethodRegistry()->FindMethod(_StatementBuffer->CStr());
            if (!existingMethod->IsEmpty() && existingMethod->GetType() == ScriptObjectType::Method) {
                _StatementBuffer->Append(_ResolveMethod(existingMethod, statementStr, currentOp).AsString());
            } else {
                _StatementBuffer->Append(_ResolveParenth(varObject, statementStr, currentOp).AsString());
            }
            continue;
        }

        // Check if we've reached a space and need to evaluate the statement.
        if (!recording && (currentChar == '#' || currentChar == ';' || _Symbol.IsOperator(currentChar) || _Symbol.IsOperator(nextChar))) {
            // Skip if we don't have anything to check.
            if (_StatementBuffer->Length() == 0) {
                continue;
            }

            std::string currentStr = _StatementBuffer->CStr();

            // Check/save the operator.
            if (_Symbol.IsOperator(currentStr)) {
                // Check if we're a double operator.
                if (_Symbol.IsOperator(nextChar)) {
                    currentStr.append(1, nextChar);
                    _CurrentCharLocation++;
                }

                currentOp = _SymbolToOp(currentStr);
                _StatementBuffer->Clear();
                continue;
            }

            // Save the value as a statement.
            ScriptObject* obj = &ScriptObject::Empty;
            obj = varObject->GetContext()->FindObject(currentStr.c_str());

            if (!IsNullObject(obj) && !obj->IsEmpty()) {
                auto retType = to_method(obj)->GetReturnType();
                if (retType != MethodReturnType::Void) {
                    if (obj->GetType() == ScriptObjectType::Method && !to_method(obj)->IsImmediate()) {
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

                    // std::cout << "STMT: " << statementStr->CStr() << " WITH VALUE = " << obj->GetValue().AsString() << std::endl;
                    auto entry = Add(obj, currentOp);
                }
            } else {
                //  std::cout << "C STMT: " << statementStr->CStr() << " WITH VALUE = " << currentStr << std::endl;
                _AddValueAndOperation(currentStr, currentOp);
            }
            _StatementBuffer->Clear();
        }
    }
}

StatementObjectEntry* ScriptStatement::Add(ScriptObject*& scriptObject, StatementOperator op)
{
    auto entry = new StatementObjectEntry();
    entry->Op = op;
    entry->Value = scriptObject;
    _Entries.push_back(entry);

    // Set the type.
    if (_Type == StatementType::None) {
        SetType(_FindType(scriptObject->GetValue()));
    } else {
        assert(_FindType(scriptObject->GetValue()) == _Type && "Attempted to assign two different types in statement");
    }

    return entry;
}

StatementObjectEntry* ScriptStatement::Add(const Variant& constantValue, StatementOperator op)
{
    auto entry = new StatementObjectEntry();
    entry->Op = op;
    entry->ConstantValue = constantValue;
    _Entries.push_back(entry);

    // Set the type.
    auto type = _FindType(constantValue);
    if (_Type == StatementType::None) {
        SetType(type);
    } else {
        assert((type == _Type) && "Attempted to assign two different types in statement");
    }

    return entry;
}

Variant ScriptStatement::Evaluate()
{
    if (_Entries.size() == 0) {
        return Variant::Empty;
    }

    // Evaluate if we only have a single entry.
    if (_Entries.size() == 1) {
        auto entry = _Entries[0];
        if (entry->Op == StatementOperator::Equals) {
           return (entry->Value.IsValid() ? entry->Value->GetValue() : entry->ConstantValue);
        }
    }

    // Go through the entries and evaluate them.
    Variant value;
    for (auto entryIt : _Entries) {
        _Apply(entryIt, value);
    }

    return value;
}

std::string ScriptStatement::EvaluateAsString()
{
    Variant value = Evaluate();

    if (_Type == StatementType::Bool) {
        return (value.GetBool() > 0 ? "true" : "false");
    }
    if (_Type == StatementType::String) {
        return value.GetStringBuffer()->CStr();
    }
    if (_Type == StatementType::Integer) {
        return NumberHelper::NumberToString(value.GetInt());
    }
    if (_Type == StatementType::Float) {
        return NumberHelper::GetFloatToString(value.GetFloat(), 6);
    }
    if (_Type == StatementType::Object) {
        return "[Object]";
    }

    return "";
}

ScriptStatement* ScriptStatement::Clone(ScriptObject* scriptObject)
{
    ScriptStatement* stat = alloc_ref(ScriptStatement);
    stat->SetType(_Type);
    stat->_String = _String;

    for (auto entryIt : _Entries) {
        // Check if we need to find a matching object.
        if (!entryIt->Value->IsEmpty()) {
            auto clonedObj = scriptObject->GetContext()->FindObject(entryIt->Value->GetName().c_str());
            assert(!clonedObj->IsEmpty() && "Unabled to process statement, invalid traversal!");

            stat->Add(clonedObj->Clone(scriptObject, false), entryIt->Op);
        } else {
            stat->Add(entryIt->ConstantValue, entryIt->Op);
        }
    }

    return stat;
}

StatementType ScriptStatement::_FindType(const Variant& value)
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

Variant ScriptStatement::_ResolveParenth(ScriptObject* varObject, StringBuffer* statementStr, StatementOperator op)
{
    // Parse out the object value.
    char currentChar = '\0';
    char previousChar = '\0';
    char nextChar = '\0';
    bool recording = false;

    // Current/last concatenating condition.
    std::string currentConcatConditionStr = "";
    std::string lastConcatConditionStr = "";

    // Current return value.
    Variant retValue;

    // Saved our current statement and then clear the buffer.
    std::string savedStatementStr = _StatementBuffer->CStr();
    _StatementBuffer->Clear();

    // Build out the argument statements.
    while (_CurrentCharLocation < statementStr->Length()) {
        previousChar = currentChar;
        currentChar = statementStr->Get(_CurrentCharLocation);
        nextChar = statementStr->Get(_CurrentCharLocation + 1);
        _CurrentCharLocation++;

        if (currentChar == '"') {
            _StatementBuffer->Append(SYMPL_STRING_TOKEN);
            recording = !recording;
        }

        // Resolve another contained value.
        if (!recording && currentChar == '(') {
            // Determine if the current statement buffer is an existing method.
            auto existingMethod = SymplVMInstance->GetMethodRegistry()->FindMethod(_StatementBuffer->CStr());
            if (!existingMethod->IsEmpty() && existingMethod->GetType() == ScriptObjectType::Method) {
                _StatementBuffer->Append(_ResolveMethod(existingMethod, statementStr, op).AsString());
            } else {
                _StatementBuffer->Append(_ResolveParenth(varObject, statementStr, op).AsString());
            }
            continue;
        }

        // Check if we're dealing with a double operator for concatenating comparisons.
        if (!recording && _Symbol.IsOperator(currentChar) && _Symbol.IsOperator(nextChar)) {
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
                    retValue = IsStatementBufferTrue(varObject);
                    lastConcatConditionStr = currentConcatConditionStr;
                    continue;
                }
            }

            // Check if we need to ensure that the entire statement is true.
            if (isConcatOperator) {
                if (lastConcatConditionStr == "&&" && retValue.GetBool()) {
                    retValue = IsStatementBufferTrue(varObject);
                    if (!retValue.GetBool()) {
                        retValue = false;
                    }
                } else if (lastConcatConditionStr == "||" && !retValue.GetBool()) {
                    retValue = IsStatementBufferTrue(varObject);
                }
                lastConcatConditionStr = currentConcatConditionStr;
                _CurrentCharLocation++;
                continue;
            }
        }

        // Build and evaluate the statement.
        if (!recording && currentChar == ')') {
            // Check if we already have an argument value.
            if (retValue.IsEmpty()) {
                retValue = GetEvalFromStatementBuffer(varObject);
            } else if (currentConcatConditionStr == "&&") {
                if (retValue.GetType() != VariantType::Bool || retValue.GetBool()) {
                    retValue = GetEvalFromStatementBuffer(varObject);
                }
            } else if (currentConcatConditionStr == "||") {
                if (retValue.GetType() != VariantType::Bool || !retValue.GetBool()) {
                    retValue = GetEvalFromStatementBuffer(varObject);
                }
            }

            // _StatementBuffer->Replace(SYMPL_STRING_TOKEN, "");

            // Restore the statement buffer.
            _StatementBuffer->Clear();
            _StatementBuffer->Append(savedStatementStr);

            // Return our value.
            return retValue;
        }

        if (recording || (!recording && currentChar != '#')) {
            _StatementBuffer->AppendByte(currentChar);
        }
    }

    // We failed to exit the while loop early!
    assert(false && "Unclosed parenthesis in statement found!");
}

Variant ScriptStatement::_ResolveMethod(ScriptObject* varObject, StringBuffer* statementStr, StatementOperator op)
{
    // Parse out the object value.
    char currentChar = '\0';
    char previousChar = '\0';
    char nextChar = '\0';
    bool recording = false;

    // Current/last concatenating condition.
    std::string currentConcatConditionStr = "";
    std::string lastConcatConditionStr = "";

    // Save args in the variant.
    std::vector<Variant> args;

    // Current argument value.
    Variant argValue;

    // Check if this is really a valid method object.
    ScriptObject* orgMethod = varObject;
    if (varObject->GetType() != ScriptObjectType::Method) {
        orgMethod = SymplVMInstance->GetMethodRegistry()->FindMethod(_StatementBuffer->CStr());
    }

    if (IsNullObject(orgMethod) || orgMethod->IsEmpty() || orgMethod->GetType() != ScriptObjectType::Method) {
        return Variant::Empty;
    }

    // std::cout << "RM: " << _StatementBuffer->CStr() << std::endl;

    // Immediate methods don't get cloned since they're already unique.
    ScriptObject* scriptObject = orgMethod;
    ScriptContext* context = alloc_ref(ScriptContext);

    if (varObject->GetContext()->IsEmpty()) {
        scriptObject = orgMethod->Clone(nullptr, true);
    } else {
        scriptObject = orgMethod->Clone(varObject->GetContext()->GetCurrentScope(), true);

        varObject->GetContext()->SetNextContext(context);
        context->SetPreviousContext(varObject->GetContext());
    }

    // Update our context.
    context->SetCaller(varObject);
    context->SetOwner(scriptObject);
    context->SetCurrentScope(to_method(scriptObject)->GetScope());
    scriptObject->SetContext(context);

    // Remove the name of the method so it's not part of the statement value.
    _StatementBuffer->Replace(orgMethod->GetCleanName().c_str(), "");

    // Saved our current statement and then clear the buffer.
    std::string savedStatementStr = _StatementBuffer->CStr();
    _StatementBuffer->Clear();

    // Build out the argument statements.
    while (_CurrentCharLocation < statementStr->Length()) {
        previousChar = currentChar;
        currentChar = statementStr->Get(_CurrentCharLocation);
        nextChar = statementStr->Get(_CurrentCharLocation + 1);
        _CurrentCharLocation++;

        if (currentChar == '"') {
            _StatementBuffer->Append(SYMPL_STRING_TOKEN);
            recording = !recording;
        }

        if (!recording && currentChar == '(') {
            // Determine if the current statement buffer is an existing method.
            auto existingMethod = SymplVMInstance->GetMethodRegistry()->FindMethod(_StatementBuffer->CStr());
            if (!existingMethod->IsEmpty() && existingMethod->GetType() == ScriptObjectType::Method) {
                _StatementBuffer->Append(_ResolveMethod(scriptObject, statementStr, op).AsString());
            } else {
                _StatementBuffer->Append(_ResolveParenth(scriptObject, statementStr, op).AsString());
            }
            continue;
        }

        // Check if we're dealing with a double operator for concatenating comparisons.
        if (!recording && _Symbol.IsOperator(currentChar) && _Symbol.IsOperator(nextChar)) {
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
                    argValue = IsStatementBufferTrue(scriptObject);
                    lastConcatConditionStr = currentConcatConditionStr;
                    continue;
                }
            }

            // Check if we need to ensure that the entire statement is true.
            if (isConcatOperator) {
                if (lastConcatConditionStr == "&&" && argValue.GetBool()) {
                    argValue = IsStatementBufferTrue(scriptObject);
                    if (!argValue.GetBool()) {
                        argValue = false;
                    }
                } else if (lastConcatConditionStr == "||" && !argValue.GetBool()) {
                    argValue = IsStatementBufferTrue(scriptObject);
                }
                lastConcatConditionStr = currentConcatConditionStr;
                _CurrentCharLocation++;
                continue;
            }
        }

        // Build and evaluate the statement.
        if (!recording && currentChar == ',' || (currentChar == ')')) {
            // Check if we already have an argument value.
            if (argValue.IsEmpty()) {
                argValue = GetEvalFromStatementBuffer(scriptObject);
                args.push_back(argValue);
            } else if (currentConcatConditionStr == "&&") {
                if (argValue.GetType() != VariantType::Bool || argValue.GetBool()) {
                    argValue = GetEvalFromStatementBuffer(scriptObject);
                }
                args.push_back(argValue);
            } else if (currentConcatConditionStr == "||") {
                if (argValue.GetType() != VariantType::Bool || !argValue.GetBool()) {
                    argValue = GetEvalFromStatementBuffer(scriptObject);
                }
                args.push_back(argValue);
            }

            argValue = Variant::Empty;
            currentConcatConditionStr = "";
            lastConcatConditionStr = "";

            _StatementBuffer->Clear();
            // _StatementBuffer->Replace(SYMPL_STRING_TOKEN, "");

            // We've completed building out the arguments.
            if (currentChar == ')') {
                // Restore the statement buffer.
                _StatementBuffer->Append(savedStatementStr);
                return to_method(scriptObject)->Evaluate(args);
            }
        }

        if (recording || (!recording && currentChar != ',' && currentChar != '#')) {
            _StatementBuffer->AppendByte(currentChar);
        }
    }

    // We failed to exit the while loop early!
    assert(false && "Unclosed call to method!");
}

Variant ScriptStatement::GetEvalFromStatementBuffer(ScriptObject* scriptObject)
{
    // Create the statement and set the string.
    SharedRef<ScriptStatement> stat = alloc_ref(ScriptStatement);
    stat->SetString(_StatementBuffer);
    _StatementBuffer->Clear();

    // Build the statement.
    stat->Build(scriptObject);
    return stat->Evaluate();
}

void ScriptStatement::_AddValueAndOperation(const std::string& value, StatementOperator op)
{
    // Handle constant value.
    const char* strVal = value.c_str();

    // Handle boolean value.
    if (strcmp(strVal, "true") == 0 || strcmp(strVal, "false") == 0) {
        SetType(StatementType::Bool);
        Add(strcmp(strVal, "true") == 0 ? true : false, op);
    } else {
        int intVal;
        float floatVal;

        if (NumberHelper::TryParseInt(strVal, &intVal)) {
            SetType(StatementType::Integer);
            Add(intVal, op);
        } else if (NumberHelper::TryParseFloat(strVal, &floatVal)) {
            SetType(StatementType::Float);
            Add(floatVal, op);
        } else {
            SetType(StatementType::String);
            Add(strVal, op);
        }
    }
}

bool ScriptStatement::IsStatementBufferTrue(ScriptObject* scriptObject)
{
    Variant evalValue = GetEvalFromStatementBuffer(scriptObject);
    if (evalValue.GetType() == VariantType::Int && evalValue.GetInt() != 0) {
        return true;
    } else if (evalValue.GetType() == VariantType::Float && evalValue.GetFloat() != 0.0f) {
        return true;
    } else if (evalValue.GetType() == VariantType::StringBuffer && evalValue.GetStringBuffer()->Length() > 0) {
        return true;
    } else if (evalValue.GetType() == VariantType::Bool && evalValue.GetBool()) {
        return true;
    } else if (evalValue.GetType() == VariantType::ScriptObject && !evalValue.GetScriptObject()->IsEmpty()) {
        return true;
    }
    return false;
}

void ScriptStatement::_Apply(StatementObjectEntry* entry, Variant& value)
{
    Variant evalValue;
    if (entry->Value.IsValid()) {
        evalValue = entry->Value->GetValue();
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

std::string ScriptStatement::GetTypeAsString() const
{
    if (_Type == StatementType::Bool) {
        return "bool";
    }
    if (_Type == StatementType::String) {
        return "string";
    }
    if (_Type == StatementType::Integer) {
        return "number";
    }
    if (_Type == StatementType::Float) {
        return "float";
    }
    if (_Type == StatementType::Object) {
        return "object";
    }
    if (_Type == StatementType::Method) {
        return "method";
    }
    if (_Type == StatementType::Void) {
        return "void";
    }
}

StatementOperator ScriptStatement::_SymbolToOp(const std::string& symbol)
{
    // Ensure the symbol is an operator we can parse.
    if (!_Symbol.IsOperator(symbol)) {
        return StatementOperator::None;
    }

    if (symbol == "=") {
        return StatementOperator::Equals;
    }
    if (symbol == "+") {
        return StatementOperator::Add;
    }
    if (symbol == "-") {
        return StatementOperator::Subtract;
    }
    if (symbol == "/") {
        return StatementOperator::Divide;
    }
    if (symbol == "*") {
        return StatementOperator::Multiply;
    }
    if (symbol == ">") {
        return StatementOperator::GreaterThan;
    }
    if (symbol == "<") {
        return StatementOperator::LessThan;
    }
    if (symbol == ">=") {
        return StatementOperator::GreaterEqualThan;
    }
    if (symbol == "<=") {
        return StatementOperator::LessEqualThan;
    }
    if (symbol == "==") {
        return StatementOperator::IsEqual2;
    }
    if (symbol == "!=") {
        return StatementOperator::NotIsEqual2;
    }

    return StatementOperator::None;
}
