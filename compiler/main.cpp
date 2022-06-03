#include <sympl/Sympl.hpp>
#include <fmt/format.h>
using namespace std;
SymplNamespace

int main()
{
    auto globalSymbolTable = SymbolTable::Alloc<SymbolTable>();

	auto context = SharedPtr<ParserContext>(new ParserContext());
	context->Create(nullptr, nullptr, "<program>");
	context->VariableSymbolTable = globalSymbolTable;

	globalSymbolTable->Set("null", ValueHandle::Null(context.Ptr()));
	globalSymbolTable->Set("true", ValueHandle::True(context.Ptr()));
    globalSymbolTable->Set("false", ValueHandle::False(context.Ptr()));
    globalSymbolTable->Set(PrintFunc, BuiltInFuncHandle::Alloc<BuiltInFuncHandle>(1, PrintFunc).Ptr());
    globalSymbolTable->Set(PrintRetFunc, BuiltInFuncHandle::Alloc<BuiltInFuncHandle>(1, PrintRetFunc).Ptr());
    globalSymbolTable->Set(InputFunc, BuiltInFuncHandle::Alloc<BuiltInFuncHandle>(1, InputFunc).Ptr());
    globalSymbolTable->Set(InputIntFunc, BuiltInFuncHandle::Alloc<BuiltInFuncHandle>(1, InputIntFunc).Ptr());
    globalSymbolTable->Set(IsNumberFunc, BuiltInFuncHandle::Alloc<BuiltInFuncHandle>(1, IsNumberFunc).Ptr());
    globalSymbolTable->Set(IsStringFunc, BuiltInFuncHandle::Alloc<BuiltInFuncHandle>(1, IsStringFunc).Ptr());
    globalSymbolTable->Set(IsListFunc, BuiltInFuncHandle::Alloc<BuiltInFuncHandle>(1, IsListFunc).Ptr());
    globalSymbolTable->Set(IsFunctionFunc, BuiltInFuncHandle::Alloc<BuiltInFuncHandle>(1, IsFunctionFunc).Ptr());
    globalSymbolTable->Set(AppendFunc, BuiltInFuncHandle::Alloc<BuiltInFuncHandle>(1, AppendFunc).Ptr());
    globalSymbolTable->Set(RemoveFunc, BuiltInFuncHandle::Alloc<BuiltInFuncHandle>(1, RemoveFunc).Ptr());
    globalSymbolTable->Set(ExtendFunc, BuiltInFuncHandle::Alloc<BuiltInFuncHandle>(1, ExtendFunc).Ptr());

    while (true) {
        std::string code;
        std::cout << "sympl> ";
        std::getline(cin, code);

        auto CodeBuffer = StringBuffer::Alloc<StringBuffer>();
        CodeBuffer->Set(code.c_str());
        CodeBuffer->Trim();

        if (CodeBuffer->Equals("exit"))
        {
            break;
        }
        if (CodeBuffer->Empty())
        {
            continue;
        }

        Lexer lexer("<stdin>", code.c_str());

        lexer.MakeTokens();
        auto tokens = lexer.GetTokens();
        auto errors = lexer.GetErrors();

        if (!errors.empty()) {
            cout << "Failed to Run!" << endl;

            for (auto& error: errors) {
                cout << error->ToString() << endl;
            }

            continue;
        }

        Parser parser(tokens);
        auto node = parser.Parse();

        if (node->Error.IsValid()) {
            cout << node->Error->ToString() << endl;
            continue;
        }

        // Run the program.
        auto program = SharedPtr<Interpreter>(new Interpreter());
        auto result = program->Visit(node->ParserNodePtr, context);

        if (result->Error.IsValid())
        {
            cout << result->Error->ToString() << endl;
        }
        else
        {
            auto ListResult = ObjectRef::CastTo<ListHandle>(result->Value.Ptr());
            if (!ListResult->Elements.empty()) {
                if (ListResult->Elements.size() == 1) {
                    cout << ListResult->Elements[0]->ToString() << endl;
                }
                else {
                    cout << result->Value->ToString() << endl;
                }
            }
        }
    }

    return 0;
}


