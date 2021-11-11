#include "ez/utils/type-conversion.h"
#include <catch2/catch_all.hpp>
#include <catch2/generators/catch_generators_range.hpp>

TEST_CASE("Input value type is signed")
{
    auto value = GENERATE(take(5, random(0, 100)));

    static_assert(std::is_same_v<decltype(value), int>, "Ensure that value is int.");
    static_assert(std::is_unsigned_v<decltype(ez::utils::toUnsigned(value))>);

    REQUIRE(ez::utils::toUnsigned(value) == static_cast<unsigned int>(value));
}

TEST_CASE("Input value type is unsigned")
{
    auto value = GENERATE(take(5, random(0u, 100u)));

    static_assert(std::is_same_v<decltype(value), unsigned int>, "Ensure that value is int.");
    static_assert(std::is_unsigned_v<decltype(ez::utils::toUnsigned(value))>);

    REQUIRE(ez::utils::toUnsigned(value) == static_cast<unsigned int>(value));
}

