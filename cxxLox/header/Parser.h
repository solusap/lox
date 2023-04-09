#ifndef __PARSER_H__
#define __PARSER_H__
#include "Token.h"
#include "Expr.h"
#include "Stmt.h"
#include <cstdio>
#include <stdexcept>
#include <vector>
#include <memory>
#include <iostream>

using std::shared_ptr;

struct Parser
{
    Parser(const std::vector<Token>& tokens) :
        tokens(tokens)
    {}

// private:
    std::vector<Token> tokens;
    int current = 0;
    shared_ptr<Expr::Expr> expression();
    shared_ptr<Expr::Expr> equality();
    shared_ptr<Expr::Expr> comparison();
    shared_ptr<Expr::Expr> term();
    shared_ptr<Expr::Expr> factor();
    shared_ptr<Expr::Expr> unary();
    shared_ptr<Expr::Expr> primary();
    shared_ptr<Expr::Expr> assignment();
    shared_ptr<Expr::Expr> or_();
    shared_ptr<Expr::Expr> and_();


    shared_ptr<Stmt::Stmt> statement();
    shared_ptr<Stmt::Stmt> printStatement();
    shared_ptr<Stmt::Stmt> expressionStatement();
    shared_ptr<Stmt::Stmt> ifStatement();
    shared_ptr<Stmt::Stmt> whileStatement();
    shared_ptr<Stmt::Stmt> forStatement();


    shared_ptr<Expr::Expr> parse1();
    std::vector<shared_ptr<Stmt::Stmt>> parse();

// helper functions
    bool check(TokenType t);
    Token advance();
    bool isAtEnd();
    Token peek();
    Token previous();
    template<typename ...T> bool match(T... args);
    Token consume(TokenType, const string& message);
    void parseError(Token const& t, string const& msg);
    void synchronize();
    shared_ptr<Stmt::Stmt> declaration();
    shared_ptr<Stmt::Stmt> varDeclaration();
    vector<shared_ptr<Stmt::Stmt>> block();
};

class ParseError : public std::runtime_error
{
public:
    ParseError(const string& errMsg) : std::runtime_error(errMsg)
    { }

};
#endif