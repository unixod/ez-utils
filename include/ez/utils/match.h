#ifndef EZ_UTILS_MATCH_H
#define EZ_UTILS_MATCH_H

#include <variant>
#include <ez/support/definitions.h>

namespace ez::utils {

template<typename... Callable>
struct Overload : Callable... {
    using Callable::operator()...;
};

// TODO: Remove the entire ifdef once clang manages to compile Overloa without deduction guidance.
#ifdef __clang__
template<typename... Callable>
Overload(Callable...) -> Overload<Callable...>;
#endif

template<typename...>
constexpr bool always_false = false;

template<typename Variant, typename... Callable>
constexpr decltype(auto) match(Variant&& v, Callable&&... func)
{
    return std::visit(
        Overload{
            std::forward<Callable>(func)...,
            [](auto... x) {
                static_assert(always_false<decltype(x)...>, "Not all alternatives are handled");
            }
        },
        std::forward<Variant>(v)
    );
}

} // namespace ez::utils

#endif // EZ_UTILS_MATCH_H
