#ifndef __Stmt__h__
#define __Stmt__h__
#include "Expr.h"
#include <vector>
#include <fmt/core.h>
#include <string>
#include <any>
#include <memory>

#include "Token.h"

using std::string;
using std::vector;
using std::shared_ptr;

namespace Stmt{
using std::any;
struct Block;
struct CLASSD;
struct Expression;
struct Function;
struct If;
struct Print;
struct Return;
struct Var;
struct While;
struct Visitor
{
    Visitor() = default;
    ~Visitor() = default;
    virtual any visitBlockStmt(Block& stmt) = 0;
    virtual any visitCLASSDStmt(CLASSD& stmt) = 0;
    virtual any visitExpressionStmt(Expression& stmt) = 0;
    virtual any visitFunctionStmt(Function& stmt) = 0;
    virtual any visitIfStmt(If& stmt) = 0;
    virtual any visitPrintStmt(Print& stmt) = 0;
    virtual any visitReturnStmt(Return& stmt) = 0;
    virtual any visitVarStmt(Var& stmt) = 0;
    virtual any visitWhileStmt(While& stmt) = 0;
};
struct Stmt  
{
    Stmt() = default;
    ~Stmt() = default;
    virtual any accept(Visitor& visitor) = 0;
};

struct Block : public Stmt
{
    vector<shared_ptr<Stmt>> statements;
    Block(vector<shared_ptr<Stmt>> statements)
    : statements(statements)
    {}

    any accept(Visitor& visitor) override {
        return visitor.visitBlockStmt(*this);
    }
};
struct CLASSD : public Stmt
{
    Token name;
    Expr::Variable superclass;
    vector<Function> methods;
    CLASSD(Token name,Expr::Variable superclass,vector<Function> methods)
    : name(name),superclass(superclass),methods(methods)
    {}

    any accept(Visitor& visitor) override {
        return visitor.visitCLASSDStmt(*this);
    }
};
struct Expression : public Stmt
{
    shared_ptr<Expr::Expr> expression;
    Expression(shared_ptr<Expr::Expr> expression)
    : expression(expression)
    {}

    any accept(Visitor& visitor) override {
        return visitor.visitExpressionStmt(*this);
    }
};
struct Function : public Stmt
{
    Token name;
    vector<Token> params;
    vector<shared_ptr<Stmt>> body;
    Function(Token name,vector<Token> params,vector<shared_ptr<Stmt>> body)
    : name(name),params(params),body(body)
    {}

    any accept(Visitor& visitor) override {
        return visitor.visitFunctionStmt(*this);
    }
};
struct If : public Stmt
{
    shared_ptr<Expr::Expr> condition;
    shared_ptr<Stmt> thenBranch;
    shared_ptr<Stmt> elseBranch;
    If(shared_ptr<Expr::Expr> condition,shared_ptr<Stmt> thenBranch,shared_ptr<Stmt> elseBranch)
    : condition(condition),thenBranch(thenBranch),elseBranch(elseBranch)
    {}

    any accept(Visitor& visitor) override {
        return visitor.visitIfStmt(*this);
    }
};
struct Print : public Stmt
{
    shared_ptr<Expr::Expr> expression;
    Print(shared_ptr<Expr::Expr> expression)
    : expression(expression)
    {}

    any accept(Visitor& visitor) override {
        return visitor.visitPrintStmt(*this);
    }
};
struct Return : public Stmt
{
    Token keyword;
    shared_ptr<Expr::Expr> value;
    Return(Token keyword,shared_ptr<Expr::Expr> value)
    : keyword(keyword),value(value)
    {}

    any accept(Visitor& visitor) override {
        return visitor.visitReturnStmt(*this);
    }
};
struct Var : public Stmt
{
    Token name;
    shared_ptr<Expr::Expr> initializer;
    Var(Token name,shared_ptr<Expr::Expr> initializer)
    : name(name),initializer(initializer)
    {}

    any accept(Visitor& visitor) override {
        return visitor.visitVarStmt(*this);
    }
};
struct While : public Stmt
{
    shared_ptr<Expr::Expr> condition;
    shared_ptr<Stmt> body;
    While(shared_ptr<Expr::Expr> condition,shared_ptr<Stmt> body)
    : condition(condition),body(body)
    {}

    any accept(Visitor& visitor) override {
        return visitor.visitWhileStmt(*this);
    }
};

}
#endif