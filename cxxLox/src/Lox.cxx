#include "Lox.h"
#include <fmt/core.h>
#include <string>
#include <vector>

using std::vector;
using std::string;

int main(int argc, char* argv[])
{
    LoxCxx loxcxx;
	if (argc > 2) {
		fmt::print("Usage LoxCxx <script>\n");
		return 64;
	}
	else if (argc == 2) {
		loxcxx.runFile(argv[1]);
	}
	else {
		loxcxx.runPrompt();
	}
	return 0;
}
