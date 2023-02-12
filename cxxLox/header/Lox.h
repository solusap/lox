#ifndef _LOX_H_
#define _LOX_H_
#include "Token.h"
#include "Scanner.h"
#include <fmt/core.h>
#include <iterator>
#include <system_error>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <streambuf>
#include <cerrno>
#include <functional>

using std::string;
using std::cin;
using std::vector;

struct Scanner;

class LoxCxx 
{
public:
    LoxCxx() {};
    bool hadError = false;
	void run(const string& source) {
        std::function<void(int, const string&)> errorHandle = [this](int line, const string& msg) {
            return this->error(line, msg);
        };
		Scanner scanner(source, errorHandle);
		vector<Token> tokens = scanner.scanTokens();

        for (auto&& token : tokens) {
            fmt::print("{}\n", token.tostring());
        }
	}

    void report(int line, const string& where, const string & msg) {
        fmt::print("[line {}] Error {}: {}\n", line, where, msg);
        hadError = true;
    }
    void error(int line, const string& msg) {
        report(line, "", msg);
    }
    
	void runFile(char* filename)
	{
        std::ifstream fin(filename, std::ios_base::in);
        string source;
        if (fin.is_open()) {
            source = string(std::istreambuf_iterator<char>(fin), std::istreambuf_iterator<char>());
        } else {
            throw(errno);
        }
        fmt::print("Get source from {}", filename);
        run(source);
        if (hadError) {
            exit(65);
        }        
	}

	void runPrompt()
	{
		for (;;) {
			fmt::print("> ");
			string str;
			if (cin >> str) {
				run(str);
			}
			
		}
	}
};



#endif