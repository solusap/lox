#ifndef __Expr__h__
#define __Expr__h__
#include <vector>
#include <fmt/core.h>
#include <string>
#include <any>
#include <memory>

#include "Token.h"

using std::string;
using std::vector;
using std::shared_ptr;

namespace Expr{
using std::any;
struct Assign;
struct Binary;
struct Call;
struct Get;
struct Grouping;
struct Literal;
struct Logical;
struct Set;
struct Super;
struct This;
struct Unary;
struct Variable;
struct Visitor
{
    Visitor() = default;
    ~Visitor() = default;
    virtual any visitAssignExpr(Assign& expr) = 0;
    virtual any visitBinaryExpr(Binary& expr) = 0;
    virtual any visitCallExpr(Call& expr) = 0;
    virtual any visitGetExpr(Get& expr) = 0;
    virtual any visitGroupingExpr(Grouping& expr) = 0;
    virtual any visitLiteralExpr(Literal& expr) = 0;
    virtual any visitLogicalExpr(Logical& expr) = 0;
    virtual any visitSetExpr(Set& expr) = 0;
    virtual any visitSuperExpr(Super& expr) = 0;
    virtual any visitThisExpr(This& expr) = 0;
    virtual any visitUnaryExpr(Unary& expr) = 0;
    virtual any visitVariableExpr(Variable& expr) = 0;
};
struct Expr  
{
    Expr() = default;
    ~Expr() = default;
    virtual any accept(Visitor& visitor) = 0;
};

struct Assign : public Expr
{
    Token name;
    shared_ptr<Expr> value;
    Assign(Token name,shared_ptr<Expr> value)
    : name(name),value(value)
    {}

    any accept(Visitor& visitor) override {
        return visitor.visitAssignExpr(*this);
    }
};
struct Binary : public Expr
{
    shared_ptr<Expr> left;
    Token oper;
    shared_ptr<Expr> right;
    Binary(shared_ptr<Expr> left,Token oper,shared_ptr<Expr> right)
    : left(left),oper(oper),right(right)
    {}

    any accept(Visitor& visitor) override {
        return visitor.visitBinaryExpr(*this);
    }
};
struct Call : public Expr
{
    shared_ptr<Expr> callee;
    Token paren;
    vector<shared_ptr<Expr>> arguments;
    Call(shared_ptr<Expr> callee,Token paren,vector<shared_ptr<Expr>> arguments)
    : callee(callee),paren(paren),arguments(arguments)
    {}

    any accept(Visitor& visitor) override {
        return visitor.visitCallExpr(*this);
    }
};
struct Get : public Expr
{
    shared_ptr<Expr> object;
    Token name;
    Get(shared_ptr<Expr> object,Token name)
    : object(object),name(name)
    {}

    any accept(Visitor& visitor) override {
        return visitor.visitGetExpr(*this);
    }
};
struct Grouping : public Expr
{
    shared_ptr<Expr> expression;
    Grouping(shared_ptr<Expr> expression)
    : expression(expression)
    {}

    any accept(Visitor& visitor) override {
        return visitor.visitGroupingExpr(*this);
    }
};
struct Literal : public Expr
{
    any value;
    Literal(any value)
    : value(value)
    {}

    any accept(Visitor& visitor) override {
        return visitor.visitLiteralExpr(*this);
    }
};
struct Logical : public Expr
{
    shared_ptr<Expr> left;
    Token oper;
    shared_ptr<Expr> right;
    Logical(shared_ptr<Expr> left,Token oper,shared_ptr<Expr> right)
    : left(left),oper(oper),right(right)
    {}

    any accept(Visitor& visitor) override {
        return visitor.visitLogicalExpr(*this);
    }
};
struct Set : public Expr
{
    shared_ptr<Expr> object;
    Token name;
    shared_ptr<Expr> value;
    Set(shared_ptr<Expr> object,Token name,shared_ptr<Expr> value)
    : object(object),name(name),value(value)
    {}

    any accept(Visitor& visitor) override {
        return visitor.visitSetExpr(*this);
    }
};
struct Super : public Expr
{
    Token keyword;
    Token method;
    Super(Token keyword,Token method)
    : keyword(keyword),method(method)
    {}

    any accept(Visitor& visitor) override {
        return visitor.visitSuperExpr(*this);
    }
};
struct This : public Expr
{
    Token keyword;
    This(Token keyword)
    : keyword(keyword)
    {}

    any accept(Visitor& visitor) override {
        return visitor.visitThisExpr(*this);
    }
};
struct Unary : public Expr
{
    Token oper;
    shared_ptr<Expr> right;
    Unary(Token oper,shared_ptr<Expr> right)
    : oper(oper),right(right)
    {}

    any accept(Visitor& visitor) override {
        return visitor.visitUnaryExpr(*this);
    }
};
struct Variable : public Expr
{
    Token name;
    Variable(Token name)
    : name(name)
    {}

    any accept(Visitor& visitor) override {
        return visitor.visitVariableExpr(*this);
    }
};

}
#endif