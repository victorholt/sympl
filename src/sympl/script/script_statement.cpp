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
            _ResolveMethod(varObject, statementStr, currentOp);
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
                currentOp = _SymbolToOp(currentStr);
                _StatementBuffer->Clear();
                continue;
            }

            // Save the value as a statement.
            ScriptObject* obj = nullptr;

            // Method arguments should not traverse within the scoop of the method
            // to determine which variable to use in the statement.
            if (varObject->HasMeta("is_method_arg") && varObject->GetMeta("is_method_arg").GetBool()) {
                obj = varObject->GetParent()->GetParent()->TraverseUpFindChildByName(currentStr.c_str());
            } else {
                obj = varObject->TraverseUpFindChildByName(currentStr.c_str());
            }

            if (!IsNullObject(obj) && !obj->IsEmpty()) {
                if (obj->GetType() == ScriptObjectType::Method) {
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
                auto entry = Add(obj, currentOp);
            } else {
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
    if (_Type == StatementType::None) {
        SetType(_FindType(constantValue));
    } else {
        assert(_FindType(constantValue) == _Type && "Attempted to assign two different types in statement");
    }

    return entry;
}

Variant ScriptStatement::Evaluate()
{
    if (_Entries.size() == 0) {
        return Variant::Empty;
    }

    // std::cout << fmt::format("Num Entries: {0}", _Entries.size()) << std::endl;

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
            auto clonedObj = scriptObject->TraverseUpFindChildByName(entryIt->Value->GetName().c_str());
            assert(!clonedObj->IsEmpty() && "Unabled to process statement, invalid traversal!");

            stat->Add(clonedObj, entryIt->Op);
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
    if (value.GetType() == VariantType::Int ||
        value.GetType() == VariantType::UnsignedInt) {
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

void ScriptStatement::_ResolveMethod(ScriptObject* varObject, StringBuffer* statementStr, StatementOperator op)
{
    // Parse out the object value.
    char currentChar = '\0';
    char previousChar = '\0';
    char nextChar = '\0';
    bool recording = false;

    // While moving through the statement if we come across
    // a method within the method we need to mark it as a new
    // scope level.
    unsigned scopeLevel = 0;

    // Save args in the variant.
    std::vector<Variant> args;

    // Check if this is really a valid object.
    auto scriptObject = varObject->TraverseUpFindChildByName(_StatementBuffer->CStr());
    _StatementBuffer->Clear();

    if (IsNullObject(scriptObject) || scriptObject->IsEmpty() || scriptObject->GetType() != ScriptObjectType::Method) {
        return;
    }

    // Build out the argument statements.
    while (_CurrentCharLocation < statementStr->Length()) {
        previousChar = currentChar;
        currentChar = statementStr->Get(_CurrentCharLocation);
        nextChar = statementStr->Get(_CurrentCharLocation + 1);
        _CurrentCharLocation++;

        // Increase our scope level as we're attempting
        // to call another method.
        if (currentChar == '(') {
            scopeLevel++;
        }

        // Build and evaluate the statement.
        if (currentChar == ',' || (currentChar == ')' && scopeLevel == 0)) {
            // Create the statement and set the string.
            SharedRef<ScriptStatement> stat = alloc_ref(ScriptStatement);
            stat->SetString(_StatementBuffer);

            // Build the statement.
            stat->Build(scriptObject);
            args.push_back(stat->Evaluate());

            _StatementBuffer->Clear();

            // We've completed building out the arguments.
            if (currentChar == ')') {
                Variant value = to_method(scriptObject)->Evaluate(args);
                Add(value, op);

                return;
            }
        }

        // Decrease our scope level as we exit a method call.
        if (currentChar == ')') {
            scopeLevel--;
        }

        _StatementBuffer->AppendByte(currentChar);
    }

    /*

    // Build out the arguments.
    while (_CurrentCharLocation < statementStr->Length()) {
        previousChar = currentChar;
        currentChar = statementStr->Get(_CurrentCharLocation);
        nextChar = statementStr->Get(_CurrentCharLocation + 1);
        _CurrentCharLocation++;

        // Check if we need to start recording a string.
        if (currentChar == '"') {
            recording = !recording;
            continue;
        }

        // End arguments and call the method.
        if (!recording && currentChar == ')') {
            // Check if we need to add an argument.
            if (_StatementBuffer->Length() > 0) {
                // Check to see if this is an object.
                auto valueStr = _StatementBuffer->CStr();
                auto scriptObject = varObject->TraverseUpFindChildByName(_StatementBuffer->CStr());
                if (!scriptObject->IsEmpty()) {
                    args.push_back(scriptObject->GetValue());
                } else {
                    int intVal;
                    float floatVal;

                    if (NumberHelper::TryParseInt(valueStr, &intVal)) {
                        args.push_back(intVal);
                    } else if (NumberHelper::TryParseFloat(valueStr, &floatVal)) {
                        args.push_back(floatVal);
                    } else {
                        args.push_back(valueStr);
                    }
                }
            }

            Variant value = to_method(scriptObject)->Evaluate(args);
            Add(value, op);

            _StatementBuffer->Clear();
            return;
        }

        if (recording || (!recording && currentChar != '#')) {
            _StatementBuffer->AppendByte(currentChar);
        }
    }*/

    // We failed to exit the while loop early!
    assert(false && "Unclosed call to method!");
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
                value.Set(evalValue.GetInt() + value.GetInt());
            }
            break;
        case (int)StatementOperator::Subtract:
            break;
        case (int)StatementOperator::Divide:
            break;
        case (int)StatementOperator::Multiply:
            break;
        case (int)StatementOperator::Mod:
            break;
        case (int)StatementOperator::GreaterThan:
            break;
        case (int)StatementOperator::LessThan:
            break;
        case (int)StatementOperator::GreaterEqualThan:
            break;
        case (int)StatementOperator::LessEqualThan:
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

    return StatementOperator::None;
}
