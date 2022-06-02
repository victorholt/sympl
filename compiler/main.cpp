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
    globalSymbolTable->Set("Print", BuiltInFuncHandle::Alloc<BuiltInFuncHandle>(1, "Print").Ptr());

    while (true) {
        std::string code;
        std::cout << "sympl> ";
        std::getline(cin, code);

        if (strcmp(code.c_str(), "exit") == 0)
        {
            break;
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
            cout << result->Value->ToString() << endl;
        }
    }

    return 0;
}


