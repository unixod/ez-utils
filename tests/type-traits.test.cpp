#include <catch2/catch_all.hpp>
#include "ez/utils/type-traits.h"


struct A {
    A(auto) {}
};

struct B {
    explicit B(auto) {}
};

TEST_CASE("concept ImplicitlyConstructible")
{
    STATIC_REQUIRE(std::is_constructible_v<A, int>);
    STATIC_REQUIRE(ez::utils::ImplicitlyConstructible<A, int>);

    STATIC_REQUIRE(std::is_constructible_v<B, int>);
    STATIC_REQUIRE(!ez::utils::ImplicitlyConstructible<B, int>);
}
