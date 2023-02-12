#ifndef _ANY_TYPE_H_
#define _ANY_TYPE_H_

#include <type_traits>
#include <any>
#include <functional>
#include <iomanip>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <string>

using std::string;

template<class T, class F>
std::pair<const std::type_index, std::function<string(std::any const&)>> to_any_visitor(F const &f)
{
    return std::pair<const std::type_index, std::function<string(std::any const&)>>{
        std::type_index(typeid(T)),
        [g = f](std::any const &a) -> string
        {
            if constexpr (std::is_void_v<T>)
                return g();
            else
                return g(std::any_cast<T const&>(a));
        }
    };
}

static std::unordered_map<
    std::type_index, 
    std::function<std::string(std::any const&)>> 
    any_visitor {
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
        // ... add more handlers for your types ...
    };

inline std::string any_tostring(const std::any& a)
{
    if (a.has_value()) {
        if (const auto it = any_visitor.find(std::type_index(a.type()));
            it != any_visitor.cend()) {
            return it->second(a);
        };
    } else {
        return "";
    }
}

template<class T, class F>
    inline void register_any_visitor(F const& f)
{
    // std::cout << "Register visitor for type "
    //           << std::quoted(typeid(T).name()) << '\n';
    any_visitor.insert(to_any_visitor<T>(f));
}

#endif