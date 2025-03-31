#include <ranges>

#include <catch2/catch_all.hpp>

#include <ez/support/std23.h>
#include <ez/utils/recursive_generator.h>

// Aux flag used in below classes to make teir int constructors explicit
// (Explicit_int_ctr{true}) or non-explicit (Explicit_int_ctr{false}).
enum struct Explicit_int_ctr : bool {};

template<typename T, typename Arg>
concept Implicitly_constructible_from = std::convertible_to<Arg, T>;


//
// A type to test ez::utils::Generator<TrivialBuiltinType>
//
using Trivial = int;

static_assert(Implicitly_constructible_from<Trivial, int>);
static_assert(std::is_trivial_v<Trivial>);

//
// A type to test ez::utils::Generator<Trivial_nonimplicitly_constructible_from_int>
//
enum struct Trivial_nonimplicitly_constructible_from_int {};

static_assert(!Implicitly_constructible_from<Trivial_nonimplicitly_constructible_from_int, int>);
static_assert(std::is_trivial_v<Trivial_nonimplicitly_constructible_from_int>);

//
// A type to test ez::utils::Generator<Nondefault_constructible<*>>
//
template<Explicit_int_ctr e>
class Nondefault_constructible {
public:
    explicit(ez::support::std23::to_underlying(e))
    Nondefault_constructible(int i) noexcept
        : val_{i}
    {}

    bool operator==(const Nondefault_constructible&) const = default;

    int value()
    {
        return val_;
    }

private:
    int val_;
};

static_assert(Implicitly_constructible_from<Nondefault_constructible<Explicit_int_ctr{false}>, int>);
static_assert(!std::is_default_constructible_v<Nondefault_constructible<Explicit_int_ctr{false}>>);
static_assert(!std::is_trivial_v<Nondefault_constructible<Explicit_int_ctr{false}>>);
static_assert(std::is_move_constructible_v<Nondefault_constructible<Explicit_int_ctr{false}>>);
static_assert(std::is_move_assignable_v<Nondefault_constructible<Explicit_int_ctr{false}>>);
static_assert(std::is_copy_constructible_v<Nondefault_constructible<Explicit_int_ctr{false}>>);
static_assert(std::is_copy_assignable_v<Nondefault_constructible<Explicit_int_ctr{false}>>);
static_assert(!Implicitly_constructible_from<Nondefault_constructible<Explicit_int_ctr{true}>, int>);
static_assert(!std::is_default_constructible_v<Nondefault_constructible<Explicit_int_ctr{true}>>);
static_assert(!std::is_trivial_v<Nondefault_constructible<Explicit_int_ctr{true}>>);
static_assert(std::is_move_constructible_v<Nondefault_constructible<Explicit_int_ctr{true}>>);
static_assert(std::is_move_assignable_v<Nondefault_constructible<Explicit_int_ctr{true}>>);
static_assert(std::is_copy_constructible_v<Nondefault_constructible<Explicit_int_ctr{true}>>);
static_assert(std::is_copy_assignable_v<Nondefault_constructible<Explicit_int_ctr{true}>>);

//
// A type to test ez::utils::Generator<Move_constructible_only>
//
template<Explicit_int_ctr e>
class Move_constructible_only {
public:
    explicit(ez::support::std23::to_underlying(e))
    Move_constructible_only(int i) noexcept
        : val_{i}
    {}

    Move_constructible_only(Move_constructible_only&&) = default;
    Move_constructible_only(const Move_constructible_only&) = delete;
    Move_constructible_only& operator = (Move_constructible_only&&) = delete;
    Move_constructible_only& operator = (const Move_constructible_only&) = delete;
    ~Move_constructible_only() = default;

    bool operator==(const Move_constructible_only&) const = default;

    int value()
    {
        return val_;
    }

private:
    int val_;
};

static_assert(Implicitly_constructible_from<Move_constructible_only<Explicit_int_ctr{false}>, int>);
static_assert(!std::is_default_constructible_v<Move_constructible_only<Explicit_int_ctr{false}>>);
static_assert(!std::is_trivial_v<Move_constructible_only<Explicit_int_ctr{false}>>);
static_assert(std::is_move_constructible_v<Move_constructible_only<Explicit_int_ctr{false}>>);
static_assert(!std::is_move_assignable_v<Move_constructible_only<Explicit_int_ctr{false}>>);
static_assert(!std::is_copy_constructible_v<Move_constructible_only<Explicit_int_ctr{false}>>);
static_assert(!std::is_copy_assignable_v<Move_constructible_only<Explicit_int_ctr{false}>>);

static_assert(!Implicitly_constructible_from<Move_constructible_only<Explicit_int_ctr{true}>, int>);
static_assert(!std::is_default_constructible_v<Move_constructible_only<Explicit_int_ctr{true}>>);
static_assert(!std::is_trivial_v<Move_constructible_only<Explicit_int_ctr{true}>>);
static_assert(std::is_move_constructible_v<Move_constructible_only<Explicit_int_ctr{true}>>);
static_assert(!std::is_move_assignable_v<Move_constructible_only<Explicit_int_ctr{true}>>);
static_assert(!std::is_copy_constructible_v<Move_constructible_only<Explicit_int_ctr{true}>>);
static_assert(!std::is_copy_assignable_v<Move_constructible_only<Explicit_int_ctr{true}>>);


using Test_type_set = std::tuple<
    Trivial,
    Trivial_nonimplicitly_constructible_from_int,
    Nondefault_constructible<Explicit_int_ctr{false}>,
    Nondefault_constructible<Explicit_int_ctr{true}>,
    Move_constructible_only<Explicit_int_ctr{false}>,
    Move_constructible_only<Explicit_int_ctr{true}>
>;

TEMPLATE_LIST_TEST_CASE("ez::utils::Recursive_generator<T> is an input range", "", Test_type_set)
{
    using G = ez::utils::Recursive_generator<TestType>;
    STATIC_REQUIRE(std::ranges::input_range<G>);
}

TEMPLATE_LIST_TEST_CASE("co_return void", "", Test_type_set)
{
    auto foo = []() -> ez::utils::Recursive_generator<TestType> {
        co_return;
    };

    REQUIRE(std::ranges::equal(foo(), std::views::empty<TestType>));
}

TEMPLATE_LIST_TEST_CASE("co_yield values (no nested generators)", "", Test_type_set)
{
    auto iota = [](int cnt) -> ez::utils::Recursive_generator<TestType> {
        for (auto i = 0; i < cnt; ++i) {
            if constexpr (Implicitly_constructible_from<TestType, int>) {
                co_yield i;
            }
            else {
                co_yield TestType{i};
            }
        }
    };

    REQUIRE(std::ranges::distance(iota(0)) == 0);
    REQUIRE(std::ranges::distance(std::as_const(iota)(0)) == 0);
    REQUIRE(std::ranges::equal(iota(0), std::views::empty<TestType>));

    auto cnt = GENERATE(take(3, random(1, 100)));
    REQUIRE(std::ranges::distance(iota(cnt)) == cnt);
    REQUIRE(std::ranges::distance(std::as_const(iota)(cnt)) == cnt);

    auto expected_seq =
        std::views::iota(0, cnt)
        | std::views::transform([](auto i){ return TestType{i}; });

    REQUIRE(std::ranges::equal(iota(cnt), expected_seq));
}

TEST_CASE("co_yeild values with nested generators")
{
    auto aaa = []() -> ez::utils::Recursive_generator<std::string_view> {
        co_yield "aaa-1";
        co_yield "aaa-2";
    };

    auto bbb = []() -> ez::utils::Recursive_generator<std::string_view> {
        co_yield "bbb-1";
        co_yield "bbb-2";
        co_yield "bbb-3";
    };

    auto eee = []() -> ez::utils::Recursive_generator<std::string_view> {
        co_return;
    };

    auto aaa_eee_foo_bbb = [&]() -> ez::utils::Recursive_generator<std::string_view> {
        co_yield aaa();
        co_yield eee();
        co_yield "foo";
        co_yield bbb();
    };

    auto expected_seq = {
        "aaa-1",
        "aaa-2",
        "foo",
        "bbb-1",
        "bbb-2",
        "bbb-3"
    };

    REQUIRE(std::ranges::equal(aaa_eee_foo_bbb(), expected_seq));
}




