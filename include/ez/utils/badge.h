#ifndef EZ_UTILS_BADGE_H
#define EZ_UTILS_BADGE_H

#include <type_traits>

namespace ez::utils {

// For the description of concept see:
//   https://news.ycombinator.com/item?id=20159216
//   https://awesomekling.github.io/Serenity-C++-patterns-The-Badge/
template<typename T>
struct Badge {
private:
    Badge() = default;
    friend T;
};

// Until C++20 Badge<X> (assuming no user provided specializations) due to its defaulted default
// constructor is treated as aggregate class which in turn enables to instantiate it outside of
// the context of X via aggregate initialization:
//   ez::utils::Badge<X>{}
// which breaks the overall idea behind Badge. Starting from C++20 ez::utils::Badge isn't anymore
// aggregate. For more info see:
//   https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p1008r1.pdf
static_assert(!std::is_aggregate_v<Badge<int>>, "ez::support::Badge requires C++20 or higher.");
static_assert(std::is_trivial_v<ez::utils::Badge<int>>);

} // namespace ez::utils

#endif // EZ_UTILS_BADGE_H
