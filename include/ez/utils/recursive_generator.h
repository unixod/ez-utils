#ifndef EZ_UTILS_RECURSIVE_GENERATOR_H
#define EZ_UTILS_RECURSIVE_GENERATOR_H

#include <cassert>
#include <coroutine>
#include <iterator>
#include <ranges>

#include "ez/support/std23.h"
#include "ez/utils/badge.h"

namespace ez::utils {

template<typename T>
class [[nodiscard]] Recursive_generator {
public:
    class Promise;
    class Iterator;
    using promise_type = Promise;

public:
    Recursive_generator(Promise& promise, utils::Badge<Promise>) noexcept
        : root_promise_{promise}
    {
        promise.set_generator(*this, utils::Badge<Recursive_generator>{});
    }

    Recursive_generator(Recursive_generator&&) = delete;
    Recursive_generator(const Recursive_generator&) = delete;
    Recursive_generator& operator = (Recursive_generator&&) = delete;
    Recursive_generator& operator = (const Recursive_generator&) = delete;
    ~Recursive_generator()
    {
        if (!leafCoroutineHandle_) {
            return;
        }

        for (auto promise = &leafCoroutineHandle_.promise(); promise != nullptr;) {
            auto next_promise = promise->get_outer_promise_ptr();
            auto handler = std::coroutine_handle<Promise>::from_promise(*promise);
            handler.destroy();
            promise = next_promise;
        };
    }

    auto begin()
    {
        return Iterator{*this, utils::Badge<Recursive_generator>{}};
    }

    auto end()
    {
        return std::default_sentinel;
    }

public:
    void advance_(utils::Badge<Recursive_generator::Iterator>)
    {
        assert(leafCoroutineHandle_ && "Handle must be valid");
        assert(!is_at_end_(utils::Badge<Recursive_generator>{}) &&
               "Going beyond the generated sequence is UB (resumming coroutine which is at final suspend point is UB).");

        leafCoroutineHandle_.resume();

        while (leafCoroutineHandle_.done()) {
            if (auto next_promise = leafCoroutineHandle_.promise().get_outer_promise_ptr()) {
                leafCoroutineHandle_.destroy();
                next_promise->set_generator(*this, utils::Badge<Recursive_generator>{});
                leafCoroutineHandle_ = std::coroutine_handle<Promise>::from_promise(*next_promise);

                assert(!leafCoroutineHandle_.done() && "Outer coroutin isn't supposed to be at final suspend point.");
                leafCoroutineHandle_.resume();
            }
            else {
                break;
            }
        }

        assert(leafCoroutineHandle_ && "Handle must be valid");
        assert(!leafCoroutineHandle_.done() || (&leafCoroutineHandle_.promise() == &root_promise_));
    }

    template<typename U>
    void set_value_(U&& val, utils::Badge<Promise>)
    {
        if constexpr (std::is_move_assignable_v<T>) {
            value_ = std::forward<U>(val);
        }
        else {
            value_.emplace(std::forward<U>(val));
        }
    }

    template<typename U>
    requires std::same_as<U, Iterator> || std::same_as<U, Promise>
        T& get_value_ref_(utils::Badge<U>) noexcept
    {
        assert(leafCoroutineHandle_ && "Handle must be valid");
        assert(!is_at_end_(utils::Badge<Recursive_generator>{}) && "Accesses element is within generated range");
        assert(value_);

        return *value_;
    }

    template<typename U>
    requires std::same_as<U, Iterator> || std::same_as<U, Promise> || std::same_as<U, Recursive_generator>
    bool is_at_end_(utils::Badge<U>) const noexcept
    {
        assert(leafCoroutineHandle_ && "Handle must be valid");
        assert(!leafCoroutineHandle_.done() || (&leafCoroutineHandle_.promise() == &root_promise_));
        return leafCoroutineHandle_.done();
    }

    [[nodiscard]]
    auto& get_leaf_promise(utils::Badge<Promise>) noexcept
    {
        assert(leafCoroutineHandle_ && "Handle must be valid");
        assert(!leafCoroutineHandle_.done() || (&leafCoroutineHandle_.promise() == &root_promise_));
        return leafCoroutineHandle_.promise();
    }

private:
    Promise& root_promise_;

    std::coroutine_handle<Promise> leafCoroutineHandle_ =
        std::coroutine_handle<Promise>::from_promise(root_promise_);

    // TODO: Think about more sophisticated storage type to get an ability to store either
    // a pointer to yeilded value (in case of co_yield [non-const-lvalue|rvalue]) or a value
    // itself (in case of co_yield const-lvalue).
    std::optional<T> value_;
};


// FIXME: Move to the Recursive_generator or hide under details_ namespace;
struct Awaiter {
    enum class Suspend : bool {};

    Suspend suspend;

    constexpr bool await_ready() const noexcept
    {
        return !support::std23::to_underlying(suspend);
    }

    constexpr void await_suspend(auto) const noexcept {}
    constexpr void await_resume() const noexcept {}
};


template<typename T>
class Recursive_generator<T>::Promise {
public:
    auto get_return_object() noexcept
    {
        return Recursive_generator<T>{*this, utils::Badge<Promise>{}};
    }

    std::suspend_never initial_suspend() const noexcept
    {
        assert(generator_);
        return {};
    }

    std::suspend_always final_suspend() const noexcept
    {
        return {};
    }

    template<typename U>
    requires (!std::same_as<std::remove_cvref_t<U>, Recursive_generator>)
    std::suspend_always yield_value(U&& val)
    {
        assert(generator_);

        generator_->set_value_(std::forward<U>(val), utils::Badge<Promise>{});
        return {};
    }

    template<typename G>
    requires std::same_as<std::remove_cvref_t<G>, Recursive_generator>
    Awaiter yield_value(G&& nested_seq)
    {
        assert(generator_);

        if (nested_seq.is_at_end_(utils::Badge<Promise>{})) {
            return {Awaiter::Suspend{false}};
        }

        generator_->set_value_(
            std::move(nested_seq.get_value_ref_(utils::Badge<Promise>{})),
            utils::Badge<Promise>{}
            );

        // Having:
        //  generator_ptr_ : a0 <- a1 <- a... <- this
        //  nested_seq     : b0 <- b1 <- b... <- leafPromise
        //
        // form:
        //  generator_ptr_ : a0 <- a1 <- a... <- this <- b0 <- b1 <- b... <- leafPromise
        //
        assert(nested_seq.root_promise_.outer_promise_ptr_ == nullptr);
        nested_seq.root_promise_.outer_promise_ptr_ = this;


        // FIXME: move this block to Recursive_generator
        generator_->leafCoroutineHandle_ =
            std::exchange(nested_seq.leafCoroutineHandle_, nullptr);

        // Actualize link to generator.
        //
        //  generator_ptr_ : a0 <- a1 <- a... <- this <- b0 <- b1 <- b... <- leafPromise
        //          ^                                                         /
        //           `--------------------link to generator------------------'
        //
        auto& leaf_promise = generator_->get_leaf_promise(utils::Badge<Promise>{});
        leaf_promise.generator_ = generator_;

        return {Awaiter::Suspend{true}};
    }

    void return_void() const noexcept
    {}

    [[noreturn]]
    void unhandled_exception()
    {
        throw;
    }

    void set_generator(Recursive_generator& g, utils::Badge<Recursive_generator>) noexcept
    {
        generator_ = &g;
    }

    auto get_outer_promise_ptr() const noexcept
    {
        return outer_promise_ptr_;
    }

private:
    Promise* outer_promise_ptr_ = nullptr;
    Recursive_generator* generator_ = nullptr;
};

template<typename T>
class Recursive_generator<T>::Iterator {
public:
    using iterator_concept = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = void;
    using reference = T&;

public:
    Iterator(Recursive_generator& generator, utils::Badge<Recursive_generator>)
        : generator_{generator}
    {}

    Iterator& operator++()
    {
        generator_.get().advance_(utils::Badge<Iterator>{});
        return *this;
    }

    void operator++(int)
    {
        operator++();
    }

    reference operator*() noexcept
    {
        return generator_.get().get_value_ref_(utils::Badge<Iterator>{});
    }

    reference operator*() const noexcept
    {
        return generator_.get().get_value_ref_(utils::Badge<Iterator>{});
    }

    bool operator == (std::default_sentinel_t) const noexcept
    {
        return generator_.get().is_at_end_(utils::Badge<Iterator>{});
    }

public:
    // FIXME: Remove this declaration as soon as it becomes possible.
    //
    // This constructor is added to adhere std::default_initializable which is currently
    // required by std::weakly_incrementable. This requirement according to defect report
    // P2325R3 is incorrect and should be absent.
    // See "Defect reports" on:
    //     https://en.cppreference.com/w/cpp/iterator/weakly_incrementable
    //
    Iterator() /*no definition & not deleted/defaulted intentionally*/;

private:
    std::reference_wrapper<Recursive_generator> generator_;
};

} // namespace ez::utils

#endif // EZ_UTILS_RECURSIVE_GENERATOR_H
