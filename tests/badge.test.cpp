#include <catch2/catch_all.hpp>
#include <ez/utils/badge.h>

class Bar;

class Foo {
public:
    void method_for_bar(ez::utils::Badge<Bar>)
    {}
};

class Bar {
public:
    template<typename X>
    auto call(X x)
    {
        x.method_for_bar(ez::utils::Badge<Bar>{});
    }
};

class Buz {
public:
    template<typename X>
    auto call(X x)
    {
        using Bar_ = std::enable_if_t<std::same_as<X, X>, Bar>;

        x.method_for_bar(ez::utils::Badge<Bar_>{});
    }
};

TEST_CASE("ez::utils::Badge is not aggregate")
{
    // Until C++20 ez::utils::Badge<X> (assuming no user provided specializations) due to its
    // defaulted default constructor is treated as aggregate class which in turn enables
    // to instantiate it outside of the context of X via aggregate initialization:
    //   ez::utils::Badge<X>{}
    // which breaks the overall idea behind Badge. Starting from C++20 ez::utils::Badge isn't
    // anymore aggregate.
    // For more info read: https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p1008r1.pdf
    STATIC_CHECK(!std::is_aggregate_v<ez::utils::Badge<Bar>>);
    STATIC_CHECK(std::is_trivial_v<ez::utils::Badge<Bar>>);
}

TEST_CASE("Bar can call Foo::method_for_bar")
{
    Bar bar;
    bar.call(Foo{});
}

TEST_CASE("Buz can't call Foo::method_for_bar")
{
// FIXME: Think how to test intended compilation fails. Note that
// here faile isn't at substitution template arguments => SFINAE
// doesn't seem helpful here.
#if 0
    Buz buz;
    buz.call(Foo{});
#endif
}


