#include "Token.h"
#include <unordered_map>
#include <stdio.h>
#include <fmt/core.h>

#include "any_type.h"
using std::string;
using std::any;

static string TokenTypeString[] = { "LEFT_PAREN", " RIGHT_PAREN", " LEFT_BRACE", " RIGHT_BRACE", "  COMMA", " DOT", " MINUS", " PLUS", " SEMICOLON", " SLASH", " STAR", "BANG", " BANG_EQUAL", "  EQUAL", " EQUAL_EQUAL", "  GREATER", " GREATER_EQUAL", "  LESS", " LESS_EQUAL", "IDENTIFIER", " STRING", " NUMBER", "AND", " CLASS", " ELSE", " FALSE", " FUN", " FOR", " IF", " NIL", " OR", "  PRINT", " RETURN", " SUPER", " THIS", " TRUE", " VAR", " WHILE", "TEOF"};


Token::Token(TokenType type, const string& lexme, const any& literal, int line) 
    : type(type), lexeme(lexme), literal(literal), line(line)
{

}

string Token::tostring() 
{
  return fmt::format("type: {}, lexme: {}, literal:{}", 
    TokenTypeString[type], lexeme, any_tostring(literal));
}