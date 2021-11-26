#ifndef EZ_UTILS_TYPE_TRAITS_H
#define EZ_UTILS_TYPE_TRAITS_H

namespace ez::utils {

template<typename T, typename Arg>
inline constexpr auto ImplicitlyConstructible = requires(Arg a, void(*f)(T)) {
    f(a);
};

} // namespace ez::utils

#endif // EZ_UTILS_TYPE_TRAITS_H
