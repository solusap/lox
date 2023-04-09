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
struct Expression;
struct Print;
struct Var;
struct Unary;
struct Block;
struct Visitor
{
    Visitor() = default;
    ~Visitor() = default;
    virtual any visitExpressionStmt(Expression& stmt) = 0;
    virtual any visitPrintStmt(Print& stmt) = 0;
    virtual any visitVarStmt(Var& stmt) = 0;
    virtual any visitUnaryStmt(Unary& stmt) = 0;
    virtual any visitBlockStmt(Block& stmt) = 0;
};
struct Stmt  
{
    Stmt() = default;
    ~Stmt() = default;
    virtual any accept(Visitor& visitor) = 0;
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
struct Var : public Stmt
{
    Token name;
    shared_ptr<Expr::Expr> initexpr;
    Var(Token name,shared_ptr<Expr::Expr> initexpr)
    : name(name),initexpr(initexpr)
    {}

    any accept(Visitor& visitor) override {
        return visitor.visitVarStmt(*this);
    }
};
struct Unary : public Stmt
{
    Token oper;
    shared_ptr<Expr::Expr> right;
    Unary(Token oper,shared_ptr<Expr::Expr> right)
    : oper(oper),right(right)
    {}

    any accept(Visitor& visitor) override {
        return visitor.visitUnaryStmt(*this);
    }
};
struct Block : public Stmt
{
    vector<shared_ptr<Stmt>> statments;
    Block(vector<shared_ptr<Stmt>> statments)
    : statments(statments)
    {}

    any accept(Visitor& visitor) override {
        return visitor.visitBlockStmt(*this);
    }
};

}
#endif