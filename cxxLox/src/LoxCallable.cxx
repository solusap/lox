#include "LoxCallable.h"
#include "Environment.h"
#include "Interpreter.h"
#include "Stmt.h"
#include "any_type.h"
#include <fmt/core.h>
#include <memory>

LoxFunction::LoxFunction(Stmt::Function& declaration, shared_ptr<Environment> closure) : declaration(declaration), closure(closure)
{
}

std::any LoxFunction::call(Interpter &interpreter, std::vector<std::any> arguments)
{
    shared_ptr<Environment> environment = std::make_shared<Environment>(closure);
    // environment->enclosing = closure;
    for (int i = 0; i < declaration.parameters.size(); i++) {
        environment->define(declaration.parameters[i].lexeme, arguments[i]);
        fmt::print("closure = {} current define  {} = {}\n", closure->toString(), declaration.parameters[i].lexeme, any_tostring(arguments[i]));
    }
    fmt::print("current env = {}\n", environment->toString());
    try {
        interpreter.executeBlock(declaration.body, environment);
    } catch (ReturnOut& t) {
        fmt::print("return value = {}\n", any_tostring(t.value));
        return t.value;
    }
    
    return std::any{};
}

int LoxFunction::arity()
{
    return declaration.parameters.size();
}

string LoxFunction::toString()
{
    return fmt::format("<fn {}>", declaration.name.lexeme);
}