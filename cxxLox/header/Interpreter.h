#ifndef __INTERPRTER_H_
#define __INTERPRTER_H_
#include "Token.h"
#include "Expr.h"
#include "Stmt.h"

#include <any>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <vector>
#include "any_type.h"
#include "Environment.h"

struct Interpter : public Expr::Visitor, public Stmt::Visitor
{

    Environment environment;
    bool isTruthy(std::any& obj);
    bool isEqual(std::any const& a, std::any const& b);

    void checkNumberOperand(Token const& oper, std::any const& operand);
    void checkNumberOperands(Token const& oper, std::any const & left, std::any const& right);
    std::any evaluate(Expr::Expr& expr);
    std::any evaluate(std::shared_ptr<Expr::Expr> expr);
    std::any visitLiteralExpr(Expr::Literal& expr) override;
    std::any visitGroupingExpr(Expr::Grouping& expr) override;
    std::any visitUnaryExpr(Expr::Unary& expr) override;
    std::any visitBinaryExpr(Expr::Binary& expr) override;
    std::any visitAssignExpr(Expr::Assign& expr) override;
    std::any visitCallExpr(Expr::Call& expr) override;
    std::any visitGetExpr(Expr::Get& expr) override;
    std::any visitLogicalExpr(Expr::Logical& expr) override;
    std::any visitSetExpr(Expr::Set& expr) override;
    std::any visitSuperExpr(Expr::Super& expr) override;
    std::any visitThisExpr(Expr::This& expr) override;
    std::any visitVariableExpr(Expr::Variable& expr) override;

    std::any visitPrintStmt(Stmt::Print& stmt) override;
    std::any visitExpressionStmt(Stmt::Expression& stmt) override;
    std::any visitVarStmt(Stmt::Var& stmt) override;
    std::any visitUnaryStmt(Stmt::Unary& stmt) override;
    std::any visitBlockStmt(Stmt::Block& block) override;

    // std::any visitCLASSDStmt(Stmt::CLASSD& stmt) override;
    // std::any visitFunctionStmt(Stmt::Function& stmt) override;
    // std::any visitIfStmt(Stmt::If& stmt) override;
    // std::any visitReturnStmt(Stmt::Return& stmt) override;
    // std::any visitWhileStmt(Stmt::While& stmt) override;
    void interpret(Expr::Expr& expression);
    void interpret(std::vector<shared_ptr<Stmt::Stmt>>& statements);
    void execute(Stmt::Stmt& stmt);
    void executeBlock(vector<shared_ptr<Stmt::Stmt>>& statements, std::shared_ptr<Environment> env);
};

#endif