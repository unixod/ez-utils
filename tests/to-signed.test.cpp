#include <catch2/catch_all.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include <ez/utils/type-conversion.h>

TEST_CASE("Input value type is signed")
{
    SECTION("Value is positive")
    {
        auto value = GENERATE(take(5, random(0, 100)));

        static_assert(std::is_same_v<decltype(value), int>, "Ensure that value is int.");
        static_assert(std::is_signed_v<decltype(ez::utils::to_signed(value))>);

        REQUIRE(ez::utils::to_signed(value) == value);
    }

    SECTION("Value is negative")
    {
        auto value = GENERATE(take(5, random(-100, 0)));

        static_assert(std::is_same_v<decltype(value), int>, "Ensure that value is int.");
        static_assert(std::is_signed_v<decltype(ez::utils::to_signed(value))>);

        REQUIRE(ez::utils::to_signed(value) == value);
    }
}

TEST_CASE("Input value type is unsigned")
{
    auto value = GENERATE(take(5, random(0u, 100u)));

    static_assert(std::is_same_v<decltype(value), unsigned int>, "Ensure that value is int.");
    static_assert(std::is_signed_v<decltype(ez::utils::to_signed(value))>);

    REQUIRE(ez::utils::to_signed(value) == static_cast<int>(value));
}

