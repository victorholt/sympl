#include <sympl/Sympl.hpp>
#include <fmt/format.h>
#include <sympl/Test/Memory/ManagedObjectTest.hpp>
using namespace std;
SymplNamespace

int main()
{
//    ManagedObjectTest managedObjectTest;
//    managedObjectTest.Run();

    while (true) {
        std::string code;
        std::cout << "sympl> ";
        std::getline(cin, code);

        if (strcmp(code.c_str(), "exit") == 0)
        {
            break;
        }

        Lexer lexer("stdin", code.c_str());

        lexer.MakeTokens();
        auto tokens = lexer.GetTokens();
        auto errors = lexer.GetErrors();

        if (!errors.empty()) {
            cout << "Failed to Run!" << endl;

            for (auto& error: errors) {
                cout << error.ToString() << endl;
            }
        }

        Parser parser(tokens);
        auto node = parser.Parse();

        if (node->Error.IsValid()) {
            cout << node->Error->ToString() << endl;
        } else {
            cout << node->ParserNodePtr->ToString() << endl;
        }

        // Run the program.
        auto program = SharedPtr<Interpreter>(new Interpreter());
        auto result = program->Visit(node->ParserNodePtr);

        cout << result->ToString() << endl;
    }

    return 0;
}


