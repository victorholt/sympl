#include "sympl/System/Sympl.hpp"
#include <fmt/format.h>
using namespace std;
SymplNamespace

int main()
{
	cout << "Create lexer" << endl;

	Lexer lexer("1 + 1");

	lexer.MakeTokens();
	auto tokens = lexer.GetTokens();
	auto errors = lexer.GetErrors();

	if (!errors.empty()) {
		cout << "Failed to Run!" << endl;

		for (auto& error : errors) {
			cout << error.ToString() << endl;
		}
	} else {
		cout << "Successfully Ran!" << endl;

		for (auto& token : tokens) {
			cout << token.ToString() << endl;
		}
	}

    return 0;
}


