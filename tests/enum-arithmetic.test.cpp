#include "ez/utils/enum-arithmetic.h"
#include <catch2/catch_all.hpp>
#include <catch2/generators/catch_generators_range.hpp>

enum class A {};

template<>
struct ez::utils::EnableEnumArithmeticFor<A>{};

TEST_CASE("Arithmetic")
{
    auto x = GENERATE(take(1, random(0, 100)));
    auto y = GENERATE(take(1, random(0, 100)));

    using namespace ez::utils::enum_arithmethic;


    static_assert(std::is_same_v<decltype(A{x} - A{y}), A>);
    static_assert(std::is_same_v<decltype(A{x} + A{y}), A>);

    static_assert(std::is_same_v<decltype(A{x} - y), A>);
    static_assert(std::is_same_v<decltype(x - A{y}), A>);

    static_assert(std::is_same_v<decltype(A{x} + y), A>);
    static_assert(std::is_same_v<decltype(x + A{y}), A>);

    static_assert(std::is_same_v<decltype(++std::declval<A&>()), A&>);
    static_assert(std::is_same_v<decltype(std::declval<A&>()++), A>);


    REQUIRE(A{x} - A{y} == A{x - y});
    REQUIRE(A{x} + A{y} == A{x + y});

    REQUIRE(A{x} - y == A{x - y});
    REQUIRE(x - A{y} == A{x - y});

    REQUIRE(A{x} + y == A{x + y});
    REQUIRE(x + A{y} == A{x + y});

    A a{x};
    REQUIRE(++a == A{++x});
    REQUIRE(a == A{x});

    A b{y};
    REQUIRE(b++ == A{y++});
    REQUIRE(b == A{y});
}
