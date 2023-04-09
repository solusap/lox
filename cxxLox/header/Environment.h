#ifndef __ENVRIONMENT_H__
#define __ENVRIONMENT_H__

#include <any>
#include <map>
#include <memory>
#include <string>
#include "Expr.h"
#include "Token.h"

struct Environment
{
    std::shared_ptr<Environment>enclosing;

    Environment();
    Environment(std::shared_ptr<Environment> enclosing);
    std::map<std::string, std::any> values;
    void define(const std::string& name, const std::any& value);
    std::any get(Token name);
    void assign(Token name, const std::any& value);
};


#endif