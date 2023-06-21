#ifndef _ANY_TYPE_H_
#define _ANY_TYPE_H_

#include <fmt/core.h>
#include <memory>
#include <type_traits>
#include <any>
#include <functional>
#include <iomanip>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <string>

using std::string;

extern std::unordered_map<
    std::type_index, 
    std::function<std::string(std::any const&)>> 
    any_visitor;

template<typename T>
bool any_is_type(std::any const& v) {
    return v.type() == typeid(T);
}

inline bool any_type_same(std::any const& a, std::any const& b) {
    return a.type() == b.type();
}

template <typename T>
bool any_is_equal(std::any const& a, std::any const& b) {
    return std::any_cast<T>(a) == std::any_cast<T>(b);
}

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

inline std::string any_tostring(const std::any& a)
{
    if (a.has_value()) {
        const auto it = any_visitor.find(std::type_index(a.type()));
        if (it != any_visitor.cend()) {
            return it->second(a);
        } else {
            return fmt::format("size = {}, unregisterd type {}\n", any_visitor.size(), a.type().name());
        }
    } else {
        return "";
    }
}

template<class T, class F>
    inline void register_any_visitor(F const& f)
{
    any_visitor.insert(to_any_visitor<T>(f));
}

#endif