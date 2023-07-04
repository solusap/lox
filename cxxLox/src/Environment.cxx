#include "Environment.h"
#include "Token.h"
#include <exception>
#include <map>
#include <stdexcept>
#include "fmt/core.h"
#include "any_type.h"

#include "unwind_backstrace.h"

std::map<Environment *, std::string> g_objTrace;

Environment::Environment() : enclosing(nullptr)
{
    std::string t = backtrace2String();
    g_objTrace.insert({this, t});
}

// Environment::Environment(const Environment& env)
// {
//     this->enclosing = env.enclosing;
//     this->values = env.values;
//     this->enclosing = nullptr;
// }

// Environment Environment::operator=(const Environment& env)
// {
//     this->enclosing = env.enclosing;
//     this->values = env.values;
//     return *this;
// }

Environment::Environment(shared_ptr<Environment> enclosing) : enclosing(enclosing)
{
}

void Environment::define(const std::string &name, const std::any &value)
{
    if (values.count(name) == 0) {
        values.insert({name, value});
    } else {
        values[name] = value;
    }
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

string Environment::toString() const
{
    string ret;
    for (auto&& v: values) {
        ret += fmt::format("{}: {}\n", v.first, any_tostring(v.second));
    }
    return ret;
}


Environment::~Environment()
{
#ifdef DEBUG_TRACE
    // backtrace();
    if (g_objTrace.count(this) != 0) {
        g_objTrace.erase(this);
    } else {
        fmt::print("{:#x} is not exist may be free ?", this);
    }
#endif
    if (this->enclosing) {
        // this->enclosing.reset();
        return;
    }
}