#ifndef EZ_UTILS_UTILS_H
#define EZ_UTILS_UTILS_H

#include <limits>
#include <cassert>
#include <type_traits>
#include <numeric>

namespace ez::utils {

/// A value based counterpart of std::make_signed_t
template<typename T>
constexpr auto toSigned(T value) noexcept
{
    static_assert(std::is_integral_v<T> || std::is_enum_v<T>,
            "A value must be of either integral of enumeration type.");

    static_assert(!std::is_same_v<T, bool>,
            "A value can't be boolean.");

    return static_cast<std::make_signed_t<T>>(value);
}

/// A value based counterpart of std::make_unsigned_t
template<typename T>
constexpr auto toUnsigned(T value) noexcept
{
    assert(value >= 0);
    static_assert(std::is_integral_v<T> || std::is_enum_v<T>,
            "A value must be of either integral of enumeration type.");

    static_assert(!std::is_same_v<T, bool>,
            "A value can't be boolean.");

    return static_cast<std::make_unsigned_t<T>>(value);
}

namespace details {

template<typename T>
const char* cstrToUint_helper(const char *ptr, T& number, const int processedDigitsCnt) noexcept
{
    static_assert(std::is_unsigned<T>::value, "");

    auto ch = *ptr;
    if (ch < '0' || ch > '9') {
        return ptr;
    }

    auto d = toUnsigned(ch - '0');

    if (processedDigitsCnt < std::numeric_limits<T>::digits10) {
        number = static_cast<T>(number * 10);
        number = static_cast<T>(number + d);
        return cstrToUint_helper(ptr+1, number, processedDigitsCnt+1);
    }
    else if (number <= (std::numeric_limits<T>::max() - d) / 10) {
        number = static_cast<T>(number * 10);
        number = static_cast<T>(number + d);
        ++ptr;
    }

    return ptr;
}

} // namespace details

/// A counterpart of std::ato* for unsigned integers with an ability to check for success.
///
/// In contrast to std::ato* this function returns a pointer to the input where parsing has stopped.
template<typename T>
const char* cstrToUint(const char *ptr, T& number) noexcept
{
    assert(number == 0);
    return details::cstrToUint_helper(ptr, number, /*processedDigitsCnt=*/0);
}

} // namespace ez::utils

#endif // EZ_UTILS_UTILS_H
