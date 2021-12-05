#include "ez/utils/badge.h"
#include <catch2/catch_all.hpp>

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
        x.method_for_bar(ez::utils::Badge<Bar>{});
    }
};


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


