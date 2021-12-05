#include <catch2/catch_all.hpp>
#include <ranges>

#include "ez/support/std23.h"
#include "ez/utils/generator.h"

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

TEMPLATE_LIST_TEST_CASE("ez::utils::Generate<T> is an input range", "", Test_type_set)
{
    using G = ez::utils::Generator<TestType>;
    STATIC_REQUIRE(std::ranges::input_range<G>);
}

TEMPLATE_LIST_TEST_CASE("Generate sequence using Generator<T>", "", Test_type_set)
{
    auto iota = [](int cnt) -> ez::utils::Generator<TestType> {
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

