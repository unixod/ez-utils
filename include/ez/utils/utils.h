#ifndef EZ_UTILS_CSTR_TO_UINT_H
#define EZ_UTILS_CSTR_TO_UINT_H

#include <limits>
#include <cassert>
#include <type_traits>
#include <string>
#include <stdexcept>
#include <fstream>
#include <stdexcept>
#include <numeric>

namespace ez::utils {

template<typename T>
constexpr auto makeUnsigned(T value) noexcept
{
    assert(value >= 0);
    static_assert(std::is_integral<T>::value || std::is_enum<T>::value, "A value must be of either integral of enumeration type.");
    static_assert(!std::is_same<T, bool>::value, "A value can't be boolean.");
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

    auto d = makeUnsigned(ch - '0');

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

template<typename T>
const char* cstrToUint(const char *ptr, T& number) noexcept
{
    assert(number == 0);
    return cstrToUint_helper(ptr, number, /*processedDigitsCnt=*/0);
}

} // namespace ez::utils

#endif // EZ_UTILS_CSTR_TO_UINT_H
