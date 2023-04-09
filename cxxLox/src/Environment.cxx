#include "Environment.h"
#include "Token.h"
#include <exception>
#include <stdexcept>
#include "fmt/core.h"
// struct Environment
// {
//     std::map<std::string, std::any> values;
//     void define(const std::string& name, const std::any& value);
    
// };

Environment::Environment() : enclosing(nullptr)
{
}

Environment::Environment(std::shared_ptr<Environment> enclosing) : enclosing(enclosing)
{
}

void Environment::define(const std::string &name, const std::any &value)
{
    values.insert({name, value});
}

std::any Environment::get(Token name)
{
    if (values.count(name.lexeme) != 0) {
        return values[name.lexeme];
    }
    if (enclosing != nullptr) {
        return enclosing->get(name);
    }
    throw std::runtime_error(fmt::format("Undefined variable '{}'.", name.lexeme));
};

void Environment::assign(Token name, const std::any &value)
{
    if (values.count(name.lexeme) != 0) {
        values[name.lexeme] = value;
        return;
    }
    if (enclosing != nullptr) {
        enclosing->assign(name, value);
        return;
    }
    throw std::runtime_error(fmt::format("Undefined variable '{}'.", name.lexeme));
}