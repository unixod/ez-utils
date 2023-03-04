#include <catch2/catch_all.hpp>
#include <variant>
#include <string>
#include <ez/utils/match.h>
#include <ez/support/std23.h>

namespace {

template<typename T>
struct Type_as_value {
    template<typename U>
    constexpr bool operator == (Type_as_value<U>) const noexcept
    {
        return std::is_same_v<T, U>;
    }
};

template<typename T>
constexpr auto tv = Type_as_value<T>{};

} // namespace <anonymous>


TEST_CASE("Overload")
{
    auto foo = ez::utils::Overload{
        [](int i){ return i * 2; },
        [](char ch){ return std::string{ch, ch}; }
    };

    REQUIRE(foo(3) == 6);
    REQUIRE(foo('a') == "aa");

    static_assert(!std::invocable<decltype(foo), double>);
    static_assert(!std::invocable<decltype(foo), double&>);
    static_assert(!std::invocable<decltype(foo), double&&>);
    static_assert(!std::invocable<decltype(foo), const double>);
    static_assert(!std::invocable<decltype(foo), const double&>);
    static_assert(!std::invocable<decltype(foo), const double&&>);

    static_assert(tv<std::invoke_result_t<decltype(foo), int>>          == tv<int>);
    static_assert(tv<std::invoke_result_t<decltype(foo), int&>>         == tv<int>);
    static_assert(tv<std::invoke_result_t<decltype(foo), int&&>>        == tv<int>);
    static_assert(tv<std::invoke_result_t<decltype(foo), const int>>    == tv<int>);
    static_assert(tv<std::invoke_result_t<decltype(foo), const int&>>   == tv<int>);
    static_assert(tv<std::invoke_result_t<decltype(foo), const int&&>>  == tv<int>);
    static_assert(tv<std::invoke_result_t<decltype(foo), char>>         == tv<std::string>);
    static_assert(tv<std::invoke_result_t<decltype(foo), char&>>        == tv<std::string>);
    static_assert(tv<std::invoke_result_t<decltype(foo), char&&>>       == tv<std::string>);
    static_assert(tv<std::invoke_result_t<decltype(foo), const char>>   == tv<std::string>);
    static_assert(tv<std::invoke_result_t<decltype(foo), const char&>>  == tv<std::string>);
    static_assert(tv<std::invoke_result_t<decltype(foo), const char&&>> == tv<std::string>);
}

TEST_CASE("Select an appropriate handler")
{
    namespace std23 = ez::support::std23;

    enum struct Foo {};
    enum struct Bar {};

    auto [val, expected] = GENERATE(table<std::variant<Foo,Bar>, std::string>({
        {Foo{123}, "Foo: 123"},
        {Bar{321}, "Bar: 321"},
    }));

    auto result = ez::utils::match(val,
        [](Foo foo) {
            return "Foo: " + std::to_string(std23::to_underlying(foo));
        },
        [](Bar bar) {
            return "Bar: " + std::to_string(std23::to_underlying(bar));
        }
    );

    REQUIRE(result == expected);
}

TEMPLATE_TEST_CASE("Return value type", "[match]", int, int&, int&&, /*const int,*/ const int&, const int&&)
{
    enum struct Foo {};

    std::variant<Foo> v;

    using Handler_return_type = TestType;

    using Match_result_type =
        decltype(
            ez::utils::match(v,
                [](Foo) -> Handler_return_type {
                    throw std::runtime_error{"No need implementation for this test case"};
                }
            )
        );

    static_assert(std::is_same_v<Match_result_type, Handler_return_type>);
}


