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
#include <sympl/script/sympl_vm.h>
#include <sympl/script/script_method.h>
#include <sympl/io/script_parser.h>
#include <sympl/core/sympl_number_helper.h>

#include <fmt/format.h>
sympl_namespaces

Interpreter::Interpreter()
{

}

Interpreter::~Interpreter()
{
    _CommandList.clear();
}

bool Interpreter::Run()
{
    for (auto entry : _CommandList) {
        entry.Statement->Build(entry.ObjectRef.Ptr());
        entry.ObjectRef->SetValue(entry.Statement->Evaluate());
    }
    return true;
}

void Interpreter::AddCommand(ScriptObject* objectRef, ScriptStatement* statement)
{
    InterpretCommandEntry entry;
    entry.ObjectRef = objectRef;
    entry.Statement = statement;
    _CommandList.push_back(entry);
}

void Interpreter::_SetReader(ScriptReader* reader)
{
    _Reader = reader;
}

void Interpreter::_Parse()
{
    if (!_Parser.IsValid()) {
        _Parser = alloc_ref(ScriptParser);
    }
    _Parser->Parse(this);
}
