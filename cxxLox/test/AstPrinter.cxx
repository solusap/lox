#include <fmt/core.h>
#include <any>
#include <initializer_list>
#include <memory>

#include "Expr.h"
#include "Stmt.h"
#include "any_type.h"
#include "Token.h"

using std::make_shared;

using std::initializer_list;
using std::any;

struct AstPrinter : public Expr::Visitor
{

    std::string print(Expr::Expr& expr) 
    {
        return any_tostring(expr.accept(*this));
    }
    std::string print(std::shared_ptr<Expr::Expr> expr) 
    {
        return any_tostring(expr->accept(*this));
    }

    any paranthesize(const string& name, initializer_list<shared_ptr<Expr::Expr>> exprs)
    {
        string ret = "(" + name;
        for (auto expr : exprs) {
            ret += " " + any_tostring(expr->accept(*this));
        }
        ret += ")";
        return any(ret);
    }

    any visitBinaryExpr(Expr::Binary& expr) override
    {
        return paranthesize(expr.oper.lexeme, {expr.left, expr.right});
    }

    any visitGroupingExpr(Expr::Grouping& expr) override
    {
        return paranthesize("group", {expr.expression});
    }

    any visitLiteralExpr(Expr::Literal& expr) override
    {
        if (!expr.value.has_value()) {
            return any("nil");
        }
        return any_tostring(expr.value);
    }
    any visitUnaryExpr(Expr::Unary& expr) override
    {
        return paranthesize(expr.oper.lexeme, {expr.right});
    }
    any visitAssignExpr(Expr::Assign& expr) { return any(); }
    any visitCallExpr(Expr::Call& expr) { return any(); }
    any visitGetExpr(Expr::Get& expr) { return any(); }
    any visitLogicalExpr(Expr::Logical& expr) { return any(); }
    any visitSetExpr(Expr::Set& expr) { return any(); }
    any visitSuperExpr(Expr::Super& expr) { return any(); }
    any visitThisExpr(Expr::This& expr) { return any(); }
    any visitVariableExpr(Expr::Variable& expr) { return any(); }
};

int main()
{
    shared_ptr<Expr::Expr> expr = make_shared<Expr::Binary>(
        make_shared<Expr::Unary>(
            Token(TokenType::MINUS, "-", any(), 1),
            make_shared<Expr::Literal>(any(123))),
        Token(TokenType::STAR, "*", any(), 1),
        make_shared<Expr::Grouping>(
            make_shared<Expr::Literal>(any(45.67))));
    
    fmt::print("{}\n", AstPrinter().print(*expr));
}