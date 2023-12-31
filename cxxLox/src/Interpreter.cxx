#include "Interpreter.h"
#include "Environment.h"
#include "Stmt.h"
#include "Token.h"
#include "any_type.h"
#include <any>
#include <fmt/core.h>
#include <exception>
#include <memory>
#include <stdexcept>
#include <chrono>
#include "LoxCallable.h"

extern std::map<Environment *, std::string> g_objTrace;

struct nativeClock : public LoxCallable
{
    int arity() override {return 0;}
    std::any call(Interpter& interpreter, vector<std::any>arguments) override {
        auto now = std::chrono::steady_clock::now();
        return std::any{now};
    }
    string toString() override {
        return "<native fn>";
    }
};

Interpter::Interpter()
{
    globals = std::make_shared<Environment>();
    shared_ptr<LoxCallable> clockFunc = std::make_shared<nativeClock>();
    globals->define("clock", std::any{clockFunc});

    environment = std::make_shared<Environment>(globals);

    register_any_visitor<shared_ptr<LoxCallable>>(
            [](shared_ptr<LoxCallable> callablePtr)->string {
        return fmt::format("{}", callablePtr->toString());
    });
}

Interpter::~Interpter()
{
    environment->enclosing.reset();
    environment.reset();
    // globals.reset();
    globals.reset();
    for (auto && p : g_objTrace) {
        fmt::print("{}\n\n", p.second);
    }
}

void RuntimeError(const Token& token, std::string const& msg)
{
    throw std::runtime_error(fmt::format("[line: {}]: '{}', {}\n", token.line, token.lexeme, msg));
}

bool Interpter::isTruthy(std::any& obj)
{
    if (!obj.has_value()) {
        return false;       // nil = false
    }
    if (obj.type() == typeid(bool)) {
        return std::any_cast<bool>(obj);
    }
    return true;                // not bool and nil true
}

bool Interpter::isEqual(const std::any &a, const std::any &b)
{
    if ((!a.has_value()) && (!b.has_value())) {
        return true;
    }
    if (!a.has_value()) {
        return false;
    }
    if (!any_type_same(a, b)) {
        return false;
    }
    if (any_is_type<std::string>(a)) {
        return any_is_equal<std::string>(a, b);
    }
    if (any_is_type<double>(a)) {
        return any_is_equal<double>(a, b);
    }
    return false;
}

void Interpter::checkNumberOperand(const Token& oper, std::any const& operand)
{
    if (any_is_type<double>(operand)) {
        return;
    }
    RuntimeError(oper, "Operand must be a number!");
}

void Interpter::checkNumberOperands(const Token& oper, std::any const& left, std::any const& right)
{
    if (any_is_type<double>(left) && any_is_type<double>(right)) {
        return;
    }
    RuntimeError(oper, "Operands must be numbers!");
}

std::any Interpter::evaluate(Expr::Expr &expr)
{
    return expr.accept(*this);
}

std::any Interpter::evaluate(std::shared_ptr<Expr::Expr> expr)
{
    return expr->accept(*this);
}

std::any Interpter::visitLiteralExpr(Expr::Literal& expr)
{
    return expr.value;
}

std::any Interpter::visitGroupingExpr(Expr::Grouping& expr)
{
    return evaluate(expr.expression);
}

std::any Interpter::visitUnaryExpr(Expr::Unary& expr)
{
    std::any right = evaluate(expr.right);
    switch (expr.oper.type) {
        case MINUS:
            checkNumberOperand(expr.oper, right);
            return -std::any_cast<double>(right);
        case BANG:
            return !(isTruthy(right));
    }
    return std::any();
}

std::any Interpter::visitBinaryExpr(Expr::Binary& expr)
{
    std::any left = evaluate(expr.left);
    std::any right = evaluate(expr.right);

    switch (expr.oper.type) {
        case TokenType::BANG_EQUAL:
            return std::any{!isEqual(left, right)};
        case TokenType::EQUAL_EQUAL:
            return std::any{isEqual(left, right)};
        case TokenType::GREATER:
            checkNumberOperands(expr.oper, left, right);
            return std::any{bool(std::any_cast<double>(left) > std::any_cast<double>(right))};
        case TokenType::GREATER_EQUAL:
            checkNumberOperands(expr.oper, left, right);
            return std::any{bool(std::any_cast<double>(left) >= std::any_cast<double>(right))};
        case TokenType::LESS:
            checkNumberOperands(expr.oper, left, right);
            return std::any{bool(std::any_cast<double>(left) < std::any_cast<double>(right))};
        case TokenType::LESS_EQUAL:
            checkNumberOperands(expr.oper, left, right);
            return std::any{bool(std::any_cast<double>(left) <= std::any_cast<double>(right))};
        case TokenType::MINUS:
            checkNumberOperands(expr.oper, left, right);
            return std::any{double(std::any_cast<double>(left) - std::any_cast<double>(right))};
        case TokenType::PLUS:
            if (any_is_type<double>(left) && any_is_type<double>(right)) {
                return std::any{double(std::any_cast<double>(left) + std::any_cast<double>(right))};
            }
            if (any_is_type<string>(left) && any_is_type<string>(right)) {
                return std::any{std::string(std::any_cast<std::string>(left) + std::any_cast<std::string>(right))};
            }
            RuntimeError(expr.oper, "Operands must be two numbers or two strings");
            break;
        case TokenType::SLASH:
            return std::any{double(std::any_cast<double>(left) / std::any_cast<double>(right))};
        case STAR:
            return std::any{double(std::any_cast<double>(left) * std::any_cast<double>(right))};
    }
    return std::any{};
}

std::any Interpter::visitAssignExpr(Expr::Assign& expr)
{
    std::any value = evaluate(expr.value);
    environment->assign(expr.name, value);
    return value;
}
std::any Interpter::visitCallExpr(Expr::Call& expr)
{
    std::any callee = evaluate(expr.callee);

    vector<std::any> arguments;
    for (auto&& arg : expr.arguments) {
        arguments.push_back(evaluate(*arg));
    }
    // need to check if the callee could be a function
    if (!any_is_type<shared_ptr<LoxCallable>>(callee)) {
        RuntimeError(expr.paren, "Can only call functions and classes.");
    }
    
    auto function = std::any_cast<shared_ptr<LoxCallable>>(callee);
    // check pass arguments numbers same as function define
    if (arguments.size() != function->arity()) {
        RuntimeError(expr.paren, fmt::format("Expected {} arguments but got {}.", function->arity(), arguments.size()));
    }
    return function->call(*this, arguments);
}

std::any Interpter::visitGetExpr(Expr::Get& expr)
{
     return std::any();
}
std::any Interpter::visitLogicalExpr(Expr::Logical& expr)
{
    std::any left = evaluate(expr.left);
    if (expr.oper.type == TokenType::OR) {
        if (isTruthy(left)) {
            return left;
        }
    } else {
        if (!isTruthy(left)) {
            return left;
        }
    }
    return evaluate(expr.right);
}
std::any Interpter::visitSetExpr(Expr::Set& expr)
{
     return std::any();
}
std::any Interpter::visitSuperExpr(Expr::Super& expr)
{
     return std::any();
}
std::any Interpter::visitThisExpr(Expr::This& expr)
{
     return std::any();
}
std::any Interpter::visitVariableExpr(Expr::Variable& expr)
{
    return environment->get(expr.name);
}


// implement statement virtual function
std::any Interpter::visitExpressionStmt(Stmt::Expression &stmt)
{
    evaluate(stmt.expression);
    return std::any{};
}

// implement statement virtual function
std::any Interpter::visitPrintStmt(Stmt::Print &stmt)
{
    std::any value = evaluate(stmt.expression);
    fmt::print("{}\n", any_tostring(value));
    return std::any();
}

std::any Interpter::visitVarStmt(Stmt::Var &stmt)
{
    std::any value;
    if (stmt.initexpr != nullptr) {
        value = this->evaluate(stmt.initexpr);
    }
    // fmt::print("var define {}: {} {}\n", stmt.name.lexeme, any_tostring(value), (void*)(this->environment));
    environment->define(stmt.name.lexeme, value);
    return std::any{};
}

// becuase C++ has no final keyword; use RAII to do some action when exception occures
template <typename F>
struct FinalAction
{
    FinalAction(F f) : _clean(f) {}
    ~FinalAction() { _clean(); }
private:
    F _clean;
};

template<typename F>
FinalAction<F> finally(F f)
{
    return FinalAction<F>(f);
}

void Interpter::executeBlock(vector<shared_ptr<Stmt::Stmt>>& statements, 
                             shared_ptr<Environment> env)
{
    shared_ptr<Environment> previous = this->environment;
    auto action = [this, &previous]() {
        this->environment = previous;
        fmt::print("finally action this->env = {}\n", this->environment->toString());
    };
    finally(action);
    this->environment = env;
    for (auto&& stmt : statements) {
        execute(*stmt);
    }
}

std::any Interpter::visitBlockStmt(Stmt::Block &block)
{
    shared_ptr<Environment> env = std::make_shared<Environment>(this->environment);
    // Environment env {this->environment};
    // shared_ptr<Environment> env2(&env,  [] (auto p) {});
    executeBlock(block.statments, env);
    return std::any{};
}

std::any Interpter::visitIfStmt(Stmt::If &stmt)
{
    std::any conditionVal = evaluate(*(stmt.condition));
    if (isTruthy(conditionVal)) {
        execute(*stmt.thenBranch);
    } else if (stmt.elseBranch) {
        execute(*stmt.thenBranch);
    }
    return std::any{};  
}

std::any Interpter::visitWhileStmt(Stmt::While& stmt)
{
    std::any conditionVal = evaluate(stmt.condition);
    while (isTruthy(conditionVal)) {
        execute(*stmt.body);
        conditionVal = evaluate(stmt.condition);
    }
    return std::any{};
}

std::any Interpter::visitFunctionStmt(Stmt::Function& stmt)
{
    std::shared_ptr<LoxCallable> func = std::make_shared<LoxFunction>(stmt, environment);
    environment->define(stmt.name.lexeme, std::any{func});
    return std::any{};
}

std::any Interpter::visitReturnStmt(Stmt::Return& stmt)
{
    std::any value {};
    if (stmt.value) {
        value = evaluate(stmt.value);
    }
    throw ReturnOut(value);
}

void Interpter::interpret(Expr::Expr &expression)
{
    std::any value = evaluate(expression);
    std::cout<<any_tostring(value)<<"\n";
}

void Interpter::execute(Stmt::Stmt& stmt) {
    stmt.accept(*this);
}

void Interpter::interpret(std::vector<shared_ptr<Stmt::Stmt>>& statements)
{
    for (auto && stmt : statements) {
        if (stmt) {
            execute(*stmt);
        }        
    }
}