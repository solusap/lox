#ifndef _SCANNER_H_
#define _SCANNER_H_


#include <string>
#include <vector>
#include <string_view>
#include <map>
#include <unordered_map>
#include <functional>
#include "Token.h"

class LoxCxx;
using std::vector;
using std::string;
using std::map;
using std::unordered_map;



struct Scanner {
    string source;
    vector<Token> tokens;
    int start;
    int line;
    int current;
    std::function<void(int, const string&)> loxerror;
    std::map<string, TokenType> keywords;
	Scanner(const string& source1, const std::function<void(int, const string&)>& loxerr);
        // : source(source), start(1), current(0), line(1), lox(lox) {}
    vector<Token> scanTokens();
    bool isAtEnd();
    void scanToken();
    char advance();
    void addToken(TokenType type, const std::any& literal);
    void addToken(TokenType type);
    bool match(char c);
    char peek();
    void getstring();
    bool isDigit(char c);
    void getNumber();
    char peekNext();
    bool isAlpha(char c);
    bool isAlphaNumeric(char c);
    void identifier();
};



#endif