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
        promise.generator_ptr = this;
    }

    Recursive_generator(Recursive_generator&&) = default;
    Recursive_generator(const Recursive_generator&) = delete;
    Recursive_generator& operator = (Recursive_generator&&) = delete;
    Recursive_generator& operator = (const Recursive_generator&) = delete;
    ~Recursive_generator()
    {
        if (!leaf_coroutine_) {
            return;
        }

        for (auto promise = &leaf_coroutine_.promise(); promise != nullptr;) {
            auto next_promise = promise->outer_promise_ptr;
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

private:
    void advance_(utils::Badge<Iterator>)
    {
        assert(leaf_coroutine_ && "Handle must be valid");
        assert(!is_at_end_() &&
               "Going beyond the generated sequence is UB (resumming coroutine which is at final suspend point is UB).");

        leaf_coroutine_.resume();

        while (leaf_coroutine_.done()) {
            if (auto next_promise = leaf_coroutine_.promise().outer_promise_ptr) {
                leaf_coroutine_.destroy();
                next_promise->generator_ptr = this;
                leaf_coroutine_ = std::coroutine_handle<Promise>::from_promise(*next_promise);

                assert(!leaf_coroutine_.done() && "Outer coroutin isn't supposed to be at final suspend point.");
                leaf_coroutine_.resume();
            }
            else {
                break;
            }
        }

        assert(leaf_coroutine_ && "Handle must be valid");
        assert(!leaf_coroutine_.done() || (&leaf_coroutine_.promise() == &root_promise_));
    }

    template<typename U>
        requires (!std::same_as<std::remove_cvref_t<U>, Recursive_generator>)
    void set_value_(U&& val, utils::Badge<Promise>)
    {
        if constexpr (std::is_move_assignable_v<T>) {
            value_ = std::forward<U>(val);
        }
        else {
            value_.emplace(std::forward<U>(val));
        }
    }

    template<typename G>
        requires std::same_as<std::remove_cvref_t<G>, Recursive_generator>
    void set_value_(G&& nested_seq, utils::Badge<Promise>)
    {
        value_ = std::move(nested_seq.value_);

        // Having:
        //
        //      this : root_promise_ <- p... <- leaf_promise (leaf_coroutine_.promise())
        //        ^                                   /
        //         `---------(generator_ptr)---------'
        //
        //      nested_seq : root_promise_' <- p'... <- leaf_promise' (leaf_coroutine_.promise()')
        //              ^                                     /
        //               `-------(generator_ptr)-------------'
        //
        // form:
        //                            original chain                      appended chain from nested_seq
        //                   .~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.    .~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.
        //      generator_ : root_promise_ <- p... <- leaf_promise <- root_promise_' <- p'... <- leaf_promise'
        //              ^                                                                              /
        //               `----------------------(generator_ptr)---------------------------------------'
        //
        assert(nested_seq.root_promise_.outer_promise_ptr == nullptr);
        nested_seq.root_promise_.outer_promise_ptr = &leaf_coroutine_.promise();
        nested_seq.leaf_coroutine_.promise().generator_ptr = this;

        leaf_coroutine_ = std::exchange(nested_seq.leaf_coroutine_, nullptr);
    }


    template<typename U>
        requires std::same_as<U, Iterator> || std::same_as<U, Promise>
    [[nodiscard]]
    T& get_value_ref_(utils::Badge<U>) noexcept
    {
        assert(leaf_coroutine_ && "Handle must be valid");
        assert(!is_at_end_() && "Accesses element is within generated range");
        assert(value_);

        return *value_;
    }

    [[nodiscard]]
    bool is_at_end_() const noexcept
    {
        assert(leaf_coroutine_ && "Handle must be valid");
        assert(!leaf_coroutine_.done() || (&leaf_coroutine_.promise() == &root_promise_));
        return leaf_coroutine_.done();
    }

private:
    Promise& root_promise_;

    std::coroutine_handle<Promise> leaf_coroutine_ =
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
        assert(generator_ptr);
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
        assert(generator_ptr);
        generator_ptr->set_value_(std::forward<U>(val), utils::Badge<Promise>{});
        return {};
    }

    template<typename G>
        requires std::same_as<std::remove_cvref_t<G>, Recursive_generator>
    Awaiter yield_value(G&& nested_seq)
    {
        assert(generator_ptr);

        if (nested_seq.is_at_end_()) {
            return {Awaiter::Suspend{false}};
        }

        generator_ptr->set_value_(std::move(nested_seq), utils::Badge<Promise>{});

        return {Awaiter::Suspend{true}};
    }

    void return_void() const noexcept
    {}

    [[noreturn]]
    void unhandled_exception()
    {
        throw;
    }

    Promise* outer_promise_ptr = nullptr;
    Recursive_generator* generator_ptr = nullptr;
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
        return generator_.get().is_at_end_();
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
