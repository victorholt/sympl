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
#include <sympl/script/resolvers/array_resolver.h>
#include <sympl/script/resolvers/eval_resolver.h>
#include <sympl/script/script_array.h>
sympl_namespaces

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
        sympl_assert(false, "Invalid array statement found!");
    }

    // If we're creating an array then the brackets will occur after our '=' sign.
    if (bracketIndex > opIndex) {
        return false;
    }

    // Find the index of the array we want to modify.
    char currentChar = '\0';
    bool recording = false;

    while (true) {
        currentChar = statementStr->Get(stmtResolver->GetCharLocation());
        stmtResolver->SetCharLocation(stmtResolver->GetCharLocation() + 1);

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
            sympl_assert(false, "Detected unclosed bracket in array assignment!");
        }

        // Check if we've encountered a quote.
        if (currentChar == '%' &&
            statementStr->PeekSearch(SYMPL_STRING_TOKEN, stmtResolver->GetCharLocation() - 1)) {
            stmtResolver->SetCharLocation(
                    stmtResolver->GetCharLocation() + strlen(SYMPL_STRING_TOKEN) - 1);
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
                               ScriptObject* destObject, StatementOperator op)
{
    // Figure out what we're attempt to resolve.
    // Case 1: [n] = x - Modify existing array
    // Case 2: = arr[n] - Resolve an array value
    // Case 3: = [] - Resolve an array

    // Find where the brackets land.
    auto statementStr = stmtResolver->GetStatementString();
    currentStr->Clear();

    // Check to see if the equal sign occurs before or after our brackets.
    // This will determine whether we're assigning a value to an array item.
    auto bracketIndex = statementStr->FirstOccurrence('[', 0);
    auto opIndex = statementStr->FirstOccurrence('=', 0);

    // We have a problem if we can't find an index for either of these.
    if (bracketIndex == -1 || opIndex == -1) {
        sympl_assert(false, "Invalid array statement found!");
    }

    // Modifying an existing array.
    if (bracketIndex < opIndex) {
        return _ModifyExistingArray(stmtResolver, currentStr, destObject, op);
    }

    // Resolve the entire string as an array.
    if (statementStr->Get(static_cast<size_t>(bracketIndex - 1)) == '#') {
        return _ResolveArray(stmtResolver, currentStr, destObject, op);
    }

    // Attempting to resolve a single value in an array.
    return _ResolveArrayValue(stmtResolver, currentStr, destObject, op);
}

Variant ArrayResolver::_ResolveArray(StatementResolver* stmtResolver, StringBuffer* currentStr,
                      ScriptObject* destObject, StatementOperator op)
{
    sympl_assert(destObject->GetType() == ScriptObjectType::Array, "Attempted to resolve non-array object as an array!");

    auto statementStr = stmtResolver->GetStatementString();
    char currentChar = '\0';
    bool recording = false;
    bool isAssocArray = false;

    Variant valueIndex;
    bool valueAssignment = _CheckValueAssignment(stmtResolver, destObject, valueIndex);
    int arrayIndex = 0;

    // String to resolve.
    auto resolveStr = mem_alloc_ref(StringBuffer);
    SharedPtr<StringBuffer> arrayAssocKey = mem_alloc_ref(StringBuffer);

    // Build out the argument statements.
    while (true) {
        currentChar = statementStr->Get(stmtResolver->GetCharLocation());
        stmtResolver->SetCharLocation(stmtResolver->GetCharLocation() + 1);

        // Check if we've reached the end.
        if (currentChar == '\0') {
            break;
        }

        // Check if we've encountered a quote.
        if (currentChar == '%' && statementStr->PeekSearch(SYMPL_STRING_TOKEN, stmtResolver->GetCharLocation() - 1)) {
            resolveStr->Append(SYMPL_STRING_TOKEN);
            stmtResolver->SetCharLocation(stmtResolver->GetCharLocation() + strlen(SYMPL_STRING_TOKEN) - 1);
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
                    sympl_assert(false, "Missing association key in array!");
                }

                EvalResolver evalResolver;
                Variant value = evalResolver.GetEvalFromStatementBuffer(resolveStr->CStr(), destObject);

                if (!isAssocArray) {
                    valueIndex.Set(arrayIndex);
                } else {
                    valueIndex.Set(arrayAssocKey->CStr());
                }

                to_array(destObject)->SetItem(valueIndex.AsString().c_str(), value);

                resolveStr->Clear();
                arrayAssocKey->Clear();
                arrayIndex++;
            }
        } else if (currentChar == ';') {
            stmtResolver->SetCharLocation(stmtResolver->GetCharLocation() - 1);

            if (valueAssignment) {
                EvalResolver evalResolver;
                Variant value = evalResolver.GetEvalFromStatementBuffer(resolveStr->CStr(), destObject);
                to_array(destObject)->SetItem(valueIndex.AsString().c_str(), value);

                return value;
            } else {
                to_array(destObject)->SetValue(destObject);
                return Variant(destObject->GetCleanName().c_str());
            }
        } else {
            if (recording || (!recording && currentChar != '#')) {
                resolveStr->AppendByte(currentChar);
            }
        }
    }

    sympl_assert(false, "Unclosed array found!");
    return Variant::Empty;
}

Variant ArrayResolver::_ResolveArrayValue(StatementResolver* stmtResolver, StringBuffer* currentStr,
                                     ScriptObject* destObject, StatementOperator op)
{
    return Variant::Empty;
}

Variant ArrayResolver::_ModifyExistingArray(StatementResolver* stmtResolver, StringBuffer* currentStr,
                             ScriptObject* destObject, StatementOperator op)
{
    return Variant::Empty;
}
