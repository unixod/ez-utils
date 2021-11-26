#include <catch2/catch_all.hpp>
#include <ranges>

#include "ez/support/std23.h"
#include "ez/utils/generator.h"
#include "ez/utils/type-traits.h"

// Aux flag used in below classes to make teir int constructors explicit
// (ExplicitIntCtr{true}) or non-explicit (ExplicitIntCtr{false}).
enum struct ExplicitIntCtr : bool {};

//
// A type to test ez::utils::Generator<TrivialBuiltinType>
//
using Trivial = int;

static_assert(ez::utils::ImplicitlyConstructible<Trivial, int>);
static_assert(std::is_trivial_v<Trivial>);

//
// A type to test ez::utils::Generator<TrivialNonImplicitlyConstructibleFromInt>
//
enum struct TrivialNonImplicitlyConstructibleFromInt {};

static_assert(!ez::utils::ImplicitlyConstructible<TrivialNonImplicitlyConstructibleFromInt, int>);
static_assert(std::is_trivial_v<TrivialNonImplicitlyConstructibleFromInt>);

//
// A type to test ez::utils::Generator<NonDefaulConstructible<*>>
//
template<ExplicitIntCtr e>
class NonDefaulConstructible {
public:
    explicit(ez::support::std23::to_underlying(e))
    NonDefaulConstructible(int i) noexcept
        : val_{i}
    {}

    bool operator==(const NonDefaulConstructible&) const = default;

    int value()
    {
        return val_;
    }

private:
    int val_;
};

static_assert(ez::utils::ImplicitlyConstructible<NonDefaulConstructible<ExplicitIntCtr{false}>, int>);
static_assert(!std::is_default_constructible_v<NonDefaulConstructible<ExplicitIntCtr{false}>>);
static_assert(!std::is_trivial_v<NonDefaulConstructible<ExplicitIntCtr{false}>>);
static_assert(std::is_move_constructible_v<NonDefaulConstructible<ExplicitIntCtr{false}>>);
static_assert(std::is_move_assignable_v<NonDefaulConstructible<ExplicitIntCtr{false}>>);
static_assert(std::is_copy_constructible_v<NonDefaulConstructible<ExplicitIntCtr{false}>>);
static_assert(std::is_copy_assignable_v<NonDefaulConstructible<ExplicitIntCtr{false}>>);
static_assert(!ez::utils::ImplicitlyConstructible<NonDefaulConstructible<ExplicitIntCtr{true}>, int>);
static_assert(!std::is_default_constructible_v<NonDefaulConstructible<ExplicitIntCtr{true}>>);
static_assert(!std::is_trivial_v<NonDefaulConstructible<ExplicitIntCtr{true}>>);
static_assert(std::is_move_constructible_v<NonDefaulConstructible<ExplicitIntCtr{true}>>);
static_assert(std::is_move_assignable_v<NonDefaulConstructible<ExplicitIntCtr{true}>>);
static_assert(std::is_copy_constructible_v<NonDefaulConstructible<ExplicitIntCtr{true}>>);
static_assert(std::is_copy_assignable_v<NonDefaulConstructible<ExplicitIntCtr{true}>>);

//
// A type to test ez::utils::Generator<MoveConstructibleOnly>
//
template<ExplicitIntCtr e>
class MoveConstructibleOnly {
public:
    explicit(ez::support::std23::to_underlying(e))
    MoveConstructibleOnly(int i) noexcept
        : val_{i}
    {}

    MoveConstructibleOnly(MoveConstructibleOnly&&) = default;
    MoveConstructibleOnly(const MoveConstructibleOnly&) = delete;
    MoveConstructibleOnly& operator = (MoveConstructibleOnly&&) = delete;
    MoveConstructibleOnly& operator = (const MoveConstructibleOnly&) = delete;
    ~MoveConstructibleOnly() = default;

    bool operator==(const MoveConstructibleOnly&) const = default;

    int value()
    {
        return val_;
    }

private:
    int val_;
};

static_assert(ez::utils::ImplicitlyConstructible<MoveConstructibleOnly<ExplicitIntCtr{false}>, int>);
static_assert(!std::is_default_constructible_v<MoveConstructibleOnly<ExplicitIntCtr{false}>>);
static_assert(!std::is_trivial_v<MoveConstructibleOnly<ExplicitIntCtr{false}>>);
static_assert(std::is_move_constructible_v<MoveConstructibleOnly<ExplicitIntCtr{false}>>);
static_assert(!std::is_move_assignable_v<MoveConstructibleOnly<ExplicitIntCtr{false}>>);
static_assert(!std::is_copy_constructible_v<MoveConstructibleOnly<ExplicitIntCtr{false}>>);
static_assert(!std::is_copy_assignable_v<MoveConstructibleOnly<ExplicitIntCtr{false}>>);

static_assert(!ez::utils::ImplicitlyConstructible<MoveConstructibleOnly<ExplicitIntCtr{true}>, int>);
static_assert(!std::is_default_constructible_v<MoveConstructibleOnly<ExplicitIntCtr{true}>>);
static_assert(!std::is_trivial_v<MoveConstructibleOnly<ExplicitIntCtr{true}>>);
static_assert(std::is_move_constructible_v<MoveConstructibleOnly<ExplicitIntCtr{true}>>);
static_assert(!std::is_move_assignable_v<MoveConstructibleOnly<ExplicitIntCtr{true}>>);
static_assert(!std::is_copy_constructible_v<MoveConstructibleOnly<ExplicitIntCtr{true}>>);
static_assert(!std::is_copy_assignable_v<MoveConstructibleOnly<ExplicitIntCtr{true}>>);


using TestTypeSet = std::tuple<
    Trivial,
    TrivialNonImplicitlyConstructibleFromInt,
    NonDefaulConstructible<ExplicitIntCtr{false}>,
    NonDefaulConstructible<ExplicitIntCtr{true}>,
    MoveConstructibleOnly<ExplicitIntCtr{false}>,
    MoveConstructibleOnly<ExplicitIntCtr{true}>
>;

TEMPLATE_LIST_TEST_CASE("ez::utils::Generate<T> is an input range", "", TestTypeSet)
{
    using G = ez::utils::Generator<TestType>;
    STATIC_REQUIRE(std::ranges::input_range<G>);
}

TEMPLATE_LIST_TEST_CASE("Generate sequence using Generator<T>", "", TestTypeSet)
{
    auto iota = [](int cnt) -> ez::utils::Generator<TestType> {
        for (auto i = 0; i < cnt; ++i) {
            if constexpr (ez::utils::ImplicitlyConstructible<TestType, int>) {
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

    auto expectedSeq =
        std::views::iota(0, cnt)
        | std::views::transform([](auto i){ return TestType{i}; });

    REQUIRE(std::ranges::equal(iota(cnt), expectedSeq));
}

