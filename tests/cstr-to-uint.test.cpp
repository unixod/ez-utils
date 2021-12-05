#include "ez/utils/type-conversion.h"
#include <catch2/catch_all.hpp>
#include <catch2/generators/catch_generators_range.hpp>

TEST_CASE("Input character sequence starts with integer lexeme")
{
    const char* str = GENERATE(
        "2799345",
        "2799345-12",
        "2799345+12",
        "2799345asdf",
        "2799345  asdf"
    );

    std::uint8_t i8num = 0;
    auto end = ez::utils::cstr_to_uint(str, i8num);
    REQUIRE(str != end);
    REQUIRE(i8num == 27);

    std::uint16_t i16num = 0;
    end = ez::utils::cstr_to_uint(str, i16num);
    REQUIRE(str != end);
    REQUIRE(i16num == 27993);

    std::uint32_t i32num = 0;
    end = ez::utils::cstr_to_uint(str, i32num);
    REQUIRE(str != end);
    REQUIRE(i32num == 2799345);
}

TEST_CASE("Input character sequence doesn't start with integer lexeme")
{
    const char* str = GENERATE(
        " 2799345",
        "\t2799345",
        "-2799345",
        "+2799345",
        "x2799345",
        "asdf2799345"
    );

    std::uint8_t i8num = 0;
    auto end = ez::utils::cstr_to_uint(str, i8num);
    REQUIRE(str == end);
    REQUIRE(i8num == 0);

    std::uint16_t i16num = 0;
    end = ez::utils::cstr_to_uint(str, i16num);
    REQUIRE(str == end);
    REQUIRE(i16num == 0);

    std::uint32_t i32num = 0;
    end = ez::utils::cstr_to_uint(str, i32num);
    REQUIRE(str == end);
    REQUIRE(i32num == 0);
}

