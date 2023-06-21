#ifndef __LOXCALLABEL_H_
#define __LOXCALLABEL_H_

#include "Environment.h"
#include "Interpreter.h"
#include "Stmt.h"
#include <memory>
#include <vector>
#include <any>
struct LoxCallable
{
    LoxCallable() = default;
    ~LoxCallable() = default;
    LoxCallable(const LoxCallable& copy) = default;
    virtual std::any call(Interpter& interpreter, std::vector<std::any>arguments) = 0;
    virtual int arity() = 0;
    virtual string toString() = 0;
};

struct LoxFunction : public LoxCallable
{
    Stmt::Function& declaration;
    Environment& closure;
    LoxFunction(Stmt::Function& declaration, Environment& closure);
    std::any call(Interpter& interpreter, std::vector<std::any>arguments) override;
    int arity() override;
    string toString() override;
};

#endif