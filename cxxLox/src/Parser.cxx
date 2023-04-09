#include "Parser.h"
#include "Expr.h"
#include "Stmt.h"
#include "Token.h"
#include <memory>

bool Parser::isAtEnd()
{
    return peek().type == TEOF;
}

Token Parser::peek()
{
    return tokens[current];
}

Token Parser::previous()
{
    return tokens[current - 1];
}

template<typename ...T>
bool Parser::match(T... tokenTypes)
{
    for (auto tts : {tokenTypes...}) {
        if(check(tts)) {
            advance();
            return true;
        }
    }
    return false;
}

Token Parser::advance()
{
    if (!isAtEnd()) {
        current++;
    }
    return previous();
}

bool Parser::check(TokenType type)
{
    if (isAtEnd()) {
        return false;
    }
    return peek().type == type;
}

shared_ptr<Expr::Expr> Parser::assignment()
{
    auto expr = equality();
    if (match<TokenType>(TokenType::EQUAL)) {
        Token equals = previous();
        auto value = assignment();
        if (std::dynamic_pointer_cast<Expr::Variable>(expr) != nullptr) {
            Token name =  std::dynamic_pointer_cast<Expr::Variable>(expr)->name;
            return std::make_shared<Expr::Assign>(name, value);
        }   
        parseError(equals, "Invalid assignment target");
    }
    return expr;
}


shared_ptr<Expr::Expr> Parser::expression()
{
    return assignment();
    // return equality();
}

// equality       → comparison ( ( "!=" | "==" ) comparison )* ;
shared_ptr<Expr::Expr> Parser::equality()
{
    shared_ptr<Expr::Expr> expr = comparison();
    while (match<TokenType>(BANG_EQUAL, EQUAL_EQUAL)) {
        Token oper = previous();
        shared_ptr<Expr::Expr> right = comparison();
        expr = std::make_shared<Expr::Binary>(expr, oper, right);
    }
    return expr;
}

// comparison     → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
shared_ptr<Expr::Expr> Parser::comparison()
{
    auto expr = term();

    while (match<TokenType>(GREATER, GREATER_EQUAL, LESS, LESS_EQUAL)) {
        Token oper = previous();
        shared_ptr<Expr::Expr> right = term();
        expr = std::make_shared<Expr::Binary>(expr, oper, right);
    }
    return expr;
}

// term           → factor ( ( "-" | "+" ) factor )* ;
shared_ptr<Expr::Expr> Parser::term()
{
    auto expr = factor();
    while (match<TokenType>(MINUS, PLUS)) {
        Token oper = previous();
        shared_ptr<Expr::Expr> right = factor();
        expr = std::make_shared<Expr::Binary>(expr, oper, right);
    }
    return expr;
}

// factor         → unary ( ( "/" | "*" ) unary )* ;
shared_ptr<Expr::Expr> Parser::factor()
{
    auto expr = unary();
    while (match<TokenType>(SLASH, STAR)) {
        Token oper = previous();
        shared_ptr<Expr::Expr> right = unary();
        expr = std::make_shared<Expr::Binary>(expr, oper, right);
    }
    return expr;
}

// unary          → ( "!" | "-" ) unary
//                | primary ;
shared_ptr<Expr::Expr> Parser::unary()
{
    if (match(BANG, MINUS)) {
        Token oper = previous();
        auto right = primary();
        return std::make_shared<Expr::Unary>(oper, right);
    }
    return primary();
}

// primary        → NUMBER | STRING | "true" | "false" | "nil"
//                | "(" expression ")" ;
shared_ptr<Expr::Expr> Parser::primary()
{
    if (match(FALSE)) {
        return std::make_shared<Expr::Literal>(std::any(false));
    }
    if (match(TRUE)) {
        return std::make_shared<Expr::Literal>(std::any(true));
    }
    if (match(NIL)) {
        return std::make_shared<Expr::Literal>(std::any());
    }
    if (match(NUMBER, STRING)) {
        return std::make_shared<Expr::Literal>(previous().literal);
    }
    if (match(IDENTIFIER)) {
        return std::make_shared<Expr::Variable>(previous());
    }  
    if (match(LEFT_PAREN)) {
        auto expr = expression();
        consume(RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_shared<Expr::Grouping>(expr);
    }
 
    parseError(peek(), "Expected expression");
    return nullptr;
}

Token Parser::consume(TokenType t, const string &message)
{
    if (check(t)) {
        return advance();
    }
    parseError(peek(), message);
}

void Parser::parseError(const Token &t, const string &msg)
{
    if (t.type == TokenType::TEOF) {
        throw ParseError(fmt::format("[line {}] Error at end.\n", t.line));
    } else {
        throw ParseError(fmt::format("[line {}] Error at {}.\n", t.line, t.lexeme));
    }
}

void Parser::synchronize()
{
    advance();
    while(!isAtEnd()) {
        if (previous().type == SEMICOLON) {
            return;
        }

        switch (peek().type) {
            case CLASS:
            case FUN:
            case VAR:
            case FOR:
            case IF:
            case WHILE:
            case PRINT:
            case RETURN:
                return;
        }
        advance();
    }
}

shared_ptr<Expr::Expr> Parser::parse1()
{
    return expression();
}

shared_ptr<Stmt::Stmt> Parser::printStatement()
{
    shared_ptr<Expr::Expr> value = expression();
    consume(TokenType::SEMICOLON, "Expected ';' after value.");
    return std::make_shared<Stmt::Print>(value);
}

shared_ptr<Stmt::Stmt> Parser::expressionStatement()
{
    shared_ptr<Expr::Expr> expr = expression();
    consume(TokenType::SEMICOLON, "Expected ';' after expression.");
    return std::make_shared<Stmt::Expression>(expr);
}

vector<shared_ptr<Stmt::Stmt>> Parser::block()
{
    vector<shared_ptr<Stmt::Stmt>> statements;
    while (!check(RIGHT_BRACE) && !isAtEnd()) {
        statements.emplace_back(declaration());
    }
    consume(TokenType::RIGHT_BRACE, "Expected '}' after block.");
    return statements;
}


shared_ptr<Stmt::Stmt> Parser::statement()
{
    if (match<TokenType>(TokenType::PRINT)) {
        return printStatement();
    }
    if (match<TokenType>(TokenType::LEFT_BRACE)) {
        return std::make_shared<Stmt::Block>(block());
    }
    return expressionStatement();
}


shared_ptr<Stmt::Stmt> Parser::varDeclaration()
{
    Token varName = consume(TokenType::IDENTIFIER, "Expect variable name!");
    shared_ptr<Expr::Expr> initValue = nullptr;
    if (match(TokenType::EQUAL)) {
        initValue = expression();
    }

    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    return std::make_shared<Stmt::Var>(varName, initValue);
}

shared_ptr<Stmt::Stmt> Parser::declaration()
{
    try {
        if (match(TokenType::VAR)) {
            auto t = varDeclaration();
            return t;
        }
        auto t = statement();
        return t;   
    } catch (ParseError& pe) {
        fmt::print("error: {}\n", pe.what());
        synchronize();
        return nullptr;
    }
}

std::vector<shared_ptr<Stmt::Stmt>> Parser::parse()
{
    std::vector<shared_ptr<Stmt::Stmt>> statements;
    while(!isAtEnd()) {
        std::shared_ptr<Stmt::Stmt> stmt = declaration();
        // if (stmt != nullptr) {
            statements.push_back(stmt);
        // }
    }
    return statements;
}