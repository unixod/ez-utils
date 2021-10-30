#include "ez/utils/badge.h"
#include <catch2/catch_all.hpp>

class Bar;

class Foo {
public:
    void methodForBar(ez::utils::Badge<Bar>)
    {}
};

class Bar {
public:
    template<typename X>
    auto call(X x)
    {
        x.methodForBar(ez::utils::Badge<Bar>{});
    }
};

class Buz {
public:
    template<typename X>
    auto call(X x)
    {
        x.methodForBar(ez::utils::Badge<Bar>{});
    }
};


TEST_CASE("Bar can call Foo::methodForBar")
{
    Bar bar;
    bar.call(Foo{});
}

TEST_CASE("Buz can't call Foo::methodForBar")
{
// FIXME: Think how to test intended compilation fails. Note that
// here faile isn't at substitution template arguments => SFINAE
// doesn't seem helpful here.
#if 0
    Buz buz;
    buz.call(Foo{});
#endif
}


