#include "sympl/System/Sympl.hpp"
#include <fmt/format.h>
using namespace std;
SymplNamespace

int main()
{
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
    ParserNodeObject node = parser.Parse();
    if (node.BaseNode.Type == ParseNodeType::Binary) {
        cout << node.BinaryNode.ToString() << endl;
    } else {
        cout << node.BaseNode.ToString() << endl;
    }

    return 0;
}


