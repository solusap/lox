#ifndef _LOX_H_
#define _LOX_H_
#include "Stmt.h"
#include "Token.h"
#include "Scanner.h"
#include "Parser.h"
#include "Interpreter.h"

#include <fmt/core.h>
#include <iterator>
#include <memory>
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
    std::unique_ptr<Parser> parser;
    Interpter interp;

public:
    LoxCxx() {};
    bool hadError = false;
	void run(const string& source) {
        std::function<void(int, const string&)> errorHandle = [this](int line, const string& msg) {
            return this->error(line, msg);
        };
		Scanner scanner(source, errorHandle);
		vector<Token> tokens = scanner.scanTokens();

        // for (auto&& token : tokens) {
        //     fmt::print("{}\n", token.tostring());
        // }
        parser = std::make_unique<Parser>(tokens);
        vector<shared_ptr<Stmt::Stmt>> statements = parser->parse();
        interp.interpret(statements);
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
        fmt::print("Lox execute '{}' result: \n", filename);
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
			if (std::getline(std::cin, str)) {
				run(str);
			}
			
		}
	}
};



#endif