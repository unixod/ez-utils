#ifndef EZ_UTILS_MATCH_H
#define EZ_UTILS_MATCH_H

#include <variant>

namespace ez::utils {

template<typename... Callable>
struct Overload : Callable... {
    using Callable::operator()...;
};

template<typename...>
constexpr bool instantiation_guard = false;

template<typename Variant, typename... Callable>
constexpr decltype(auto) match(Variant&& v, Callable&&... func)
{
    return std::visit(
        Overload{
            std::forward<Callable>(func)...,
            [](auto... x) {
                static_assert(instantiation_guard<decltype(x)...>, "Not all alternatives are handled");
            }
        },
        std::forward<Variant>(v)
    );
}

} // namespace ez::utils

#endif // EZ_UTILS_UTILS_H
