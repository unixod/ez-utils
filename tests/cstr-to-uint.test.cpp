#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include <ez/utils/type-conversion.h>

TEST_CASE("Input character sequence starts with integer lexeme")
{
    const char* str = GENERATE(
        "2799345",
        "2799345-12",
        "2799345+12",
        "2799345asdf",
        "2799345  asdf"
    );

    const auto [end1, i8num] = ez::utils::cstr_to_uint<std::uint8_t>(str);
    REQUIRE(str != end1);
    REQUIRE(i8num == 27);

    const auto [end2, i16num] = ez::utils::cstr_to_uint<std::uint16_t>(str);
    REQUIRE(str != end2);
    REQUIRE(i16num == 27993);

    const auto [end3, i32num] = ez::utils::cstr_to_uint<std::uint32_t>(str);
    REQUIRE(str != end3);
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

    const auto [end1, i8num] = ez::utils::cstr_to_uint<std::uint8_t>(str);
    REQUIRE(str == end1);
    REQUIRE(i8num == 0);

    const auto [end2, i16num] = ez::utils::cstr_to_uint<std::uint16_t>(str);
    REQUIRE(str == end2);
    REQUIRE(i16num == 0);

    const auto [end3, i32num] = ez::utils::cstr_to_uint<std::uint32_t>(str);
    REQUIRE(str == end3);
    REQUIRE(i32num == 0);
}

