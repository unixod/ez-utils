#ifndef EZ_UTILS_BADGE_H
#define EZ_UTILS_BADGE_H

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

} // namespace ez::utils

#endif // EZ_UTILS_BADGE_H
