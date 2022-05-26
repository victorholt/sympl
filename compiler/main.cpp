#include <sympl/Sympl.hpp>
#include <fmt/format.h>
#include <sympl/Test/Memory/ManagedObjectTest.hpp>
using namespace std;
SymplNamespace

int main()
{
//    ManagedObjectTest managedObjectTest;
//    managedObjectTest.Run();

    std::string code;
    std::cout << "sympl> ";
	std::getline(cin, code);

	Lexer lexer("stdin", code.c_str());

	lexer.MakeTokens();
	auto tokens = lexer.GetTokens();
	auto errors = lexer.GetErrors();

	if (!errors.empty()) {
		cout << "Failed to Run!" << endl;

		for (auto& error : errors) {
			cout << error.ToString() << endl;
		}
	}

    Parser parser(tokens);
    auto node = parser.Parse();

    if (node->Error.IsValid()) {
        cout << node->Error->ToString() << endl;
    } else {
        cout << static_cast<ParserNumberNode*>(node->ParserNode.Ptr())->ToString() << endl;
    }

    return 0;
}


