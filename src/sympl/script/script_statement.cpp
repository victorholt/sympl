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
#include <sympl/core/sympl_number_helper.h>
sympl_namespaces

StatementObjectEntry* ScriptStatement::Add(ScriptObject*& scriptObject, StatementOperator op)
{
    auto entry = new StatementObjectEntry();
    entry->Op = op;
    entry->Value = scriptObject;
    _Entries.push_back(entry);

    // Set the type.
    if (_Type == StatementType::None) {
        SetType(scriptObject->GetValue()->GetType());
    } else {
        assert(scriptObject->GetValue()->GetType() == _Type && "Attempted to assign two different types in statement");
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

const Variant& ScriptStatement::Evaluate()
{
    assert(_Entries.size() > 0 && "Unable to evaluate without a statement");

    // Evaluate if we only have a single entry.
    if (_Entries.size() == 1) {
        auto entry = _Entries[0];
        if (entry->Op == StatementOperator::Equals) {
        //    return (entry.Value->IsValid() ? entry.Value->Ptr() : entry.ConstantValue);
            return entry->ConstantValue;
        }
    }

    // Go through the entries and evaluate them.
    Variant value;
    for (auto entryIt : _Entries) {
        _Apply(entryIt, value);
    }

    return Variant::Empty;
}

std::string ScriptStatement::EvaluateAsString()
{
    Variant value = Evaluate();

    if (_Type == StatementType::Bool) {
        return (value.GetShort() > 0 ? "true" : "false");
    }
    if (_Type == StatementType::String) {
        return value.GetStringBuffer()->CStr();
    }
    if (_Type == StatementType::Integer) {
        return NumberHelper::NumberToString(value.GetLong());
    }
    if (_Type == StatementType::Float) {
        return NumberHelper::GetFloatToString(value.GetFloat(), 11);
    }
    if (_Type == StatementType::Object) {
        return "[Object]";
    }

    return "";
}

StatementType ScriptStatement::_FindType(const Variant& value)
{
    // if (value.GetType() == VariantType::Bool) {
    //     return StatementType::Bool;
    // }
    if (value.GetType() == VariantType::Short ||
        value.GetType() == VariantType::UnsignedShort ||
        value.GetType() == VariantType::Int ||
        value.GetType() == VariantType::UnsignedInt ||
        value.GetType() == VariantType::Long ||
        value.GetType() == VariantType::UnsignedLong) {
        return StatementType::Integer;
    }
    if (value.GetType() == VariantType::Float ||
        value.GetType() == VariantType::Double) {
        return StatementType::Float;
    }
    if (value.GetType() == VariantType::StringBuffer) {
        return StatementType::String;
    }

    return StatementType::Object;
}

void ScriptStatement::_Apply(StatementObjectEntry* entry, Variant& value)
{
    Variant evalValue;
    if (entry->Value.IsValid()) {
        evalValue = entry->Value->GetValue()->Evaluate();
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
                value.Set(evalValue.GetLong() + value.GetLong());
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
}