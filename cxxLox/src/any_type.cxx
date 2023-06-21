#include "any_type.h"

std::unordered_map<
    std::type_index, 
    std::function<std::string(std::any const&)>> 
    any_visitor = {
        to_any_visitor<void>([]() -> string { return "{}"; }),
        to_any_visitor<int>([](int x) -> string { return std::to_string(x); }),
        to_any_visitor<unsigned>([](unsigned x) -> string { return std::to_string(x); }),
        to_any_visitor<float>([](float x) -> string { return std::to_string(x); }),
        to_any_visitor<double>([](double x) -> string { return std::to_string(x); }),
        to_any_visitor<char const*>([](char const *s) -> string
            { return std::string(s); }),
        to_any_visitor<decltype(nullptr)>([](std::nullptr_t x) -> string
            { return "nullptr"; }),
        to_any_visitor<string>([](const string& s) -> string
            { return s; }),
        to_any_visitor<bool>([](bool x) -> string
            { return x ? "true" : "false"; }),
        // ... add more handlers for your types ...
    };