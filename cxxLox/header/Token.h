#ifndef _TOKEN_H_
#define _TOKEN_H_
#include <string>
#include <any>

using std::string;

enum TokenType {
  // Single-character tokens.
  LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
  COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

  // One or two character tokens.
  BANG, BANG_EQUAL,
  EQUAL, EQUAL_EQUAL,
  GREATER, GREATER_EQUAL,
  LESS, LESS_EQUAL,

  // Literals.
  IDENTIFIER, STRING, NUMBER,

  // Keywords.
  AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
  PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,

  TEOF,
};

struct Token
{
    TokenType type;
    string lexeme;
    std::any literal;
    int line;

    Token(TokenType type, const string& lexme, const std::any& literal, int line);

    string tostring();
};
#endif