#ifndef EZ_UTILS_ENUM_ARITHMETIC_H
#define EZ_UTILS_ENUM_ARITHMETIC_H

#include <concepts>
#include <type_traits>
#include <ez/support/std23.h>

namespace ez::utils {

template<typename E>
    requires ez::support::std23::is_scoped_enum_v<E>
inline constexpr bool enable_enum_arithmetic = false;

template<typename E>
concept Enum_with_enabled_arith = enable_enum_arithmetic<E>;

inline namespace enum_arithmethic {

template<Enum_with_enabled_arith E>
constexpr auto operator + (E lhs, E rhs) noexcept
{
    namespace std23 = ez::support::std23;
    return E{std23::to_underlying(lhs) + std23::to_underlying(rhs)};
}

template<Enum_with_enabled_arith E>
constexpr auto operator - (E lhs, E rhs) noexcept
{
    namespace std23 = ez::support::std23;
    return E{std23::to_underlying(lhs) - std23::to_underlying(rhs)};
}

template<Enum_with_enabled_arith E>
constexpr E operator + (E lhs, std::integral auto rhs) noexcept
{
    namespace std23 = ez::support::std23;
    return E{std23::to_underlying(lhs) + rhs};
}

template<Enum_with_enabled_arith E>
constexpr E operator + (std::integral auto lhs, E rhs) noexcept
{
    namespace std23 = ez::support::std23;
    return E{lhs + std23::to_underlying(rhs)};
}

template<Enum_with_enabled_arith E>
constexpr E operator - (std::integral auto lhs, E rhs) noexcept
{
    namespace std23 = ez::support::std23;
    return E{lhs - std23::to_underlying(rhs)};
}

template<Enum_with_enabled_arith E>
constexpr E operator - (E lhs, std::integral auto rhs) noexcept
{
    namespace std23 = ez::support::std23;
    return E{std23::to_underlying(lhs) - rhs};
}

template<Enum_with_enabled_arith E>
constexpr E& operator++(E& r) noexcept
{
    ++reinterpret_cast<std::underlying_type_t<E>&>(r);
    return r;
}

template<Enum_with_enabled_arith E>
constexpr E operator++(E& r, int) noexcept
{
    auto old = r;
    ++r;
    return old;
}

} // inline namespace enum_arithmetic
} // ez::utils


#endif // EZ_UTILS_ENUM_ARITHMETIC_H
