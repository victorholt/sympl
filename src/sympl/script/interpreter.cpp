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
#include <sympl/script/interpreter.h>
#include <sympl/script/script_vm.h>
#include <sympl/script/script_method.h>
#include <sympl/script/script_parser.h>
#include <sympl/util/number_helper.h>
#include <sympl/script/statement_resolver.h>

#include <fmt/format.h>
sympl_namespaces

Interpreter::Interpreter()
{

}

void Interpreter::__Construct()
{

}

bool Interpreter::Run()
{
    SharedPtr<StatementResolver> resolver = alloc_ref(StatementResolver);

    for (auto entry : _CommandList) {
        if (entry.ObjectRef->GetType() != ScriptObjectType::Method) {
            entry.ObjectRef->SetValue(resolver->Resolve(entry.StatementStr->CStr(), entry.ObjectRef.Ptr()));
        } else {
            resolver->Resolve(entry.StatementStr->CStr(), entry.ObjectRef.Ptr());
        }
    }
    return true;
}

void Interpreter::AddCommand(ScriptObject* objectRef, const char* stmtStr)
{
    InterpretCommandEntry entry;
    entry.ObjectRef     = objectRef;
    entry.StatementStr  = mem_alloc(StringBuffer);
    entry.StatementStr->Append(stmtStr);
    _CommandList.push_back(entry);
}

void Interpreter::_SetReader(ScriptReader* reader)
{
    _Reader = reader;
}

void Interpreter::_Parse()
{
    if (!_Parser.IsValid()) {
        _Parser = mem_alloc(ScriptParser);
    }
    _Parser->Parse(this);
    _Parser.Release();
}

bool Interpreter::Release()
{
    for (auto entryIt : _CommandList) {
        entryIt.StatementStr.Release();
        entryIt.ObjectRef.Release();
    }
    _CommandList.clear();

    return true;
}
