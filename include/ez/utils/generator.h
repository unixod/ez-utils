#ifndef EZ_UTILS_GENERATOR_H
#define EZ_UTILS_GENERATOR_H

#include <cassert>
#include <coroutine>
#include <iterator>
#include <ranges>

#include "ez/utils/badge.h"

namespace ez::utils {

template<typename T>
class Generator {
public:
    class Promise;
    class Iterator;
    using promise_type = Promise;

public:
    Generator(std::coroutine_handle<Promise> coroutineHandle, ez::utils::Badge<Promise>)
        : coroutineHandle_{std::move(coroutineHandle)}
    {}

    Generator(Generator&&) = delete;
    Generator(const Generator&) = delete;
    Generator& operator = (Generator&&) = delete;
    Generator& operator = (const Generator&) = delete;
    ~Generator()
    {
        assert(coroutineHandle_);
        coroutineHandle_.destroy();
    }

    auto begin()
    {
        return Iterator{coroutineHandle_, ez::utils::Badge<Generator>{}};
    }

    auto end()
    {
        return std::default_sentinel;
    }

private:
    std::coroutine_handle<Promise> coroutineHandle_;
};

template<typename T>
class Generator<T>::Promise {
public:
    auto get_return_object()
    {
        return Generator<T>{
            std::coroutine_handle<Promise>::from_promise(*this),
            ez::utils::Badge<Promise>{}
        };
    }

    std::suspend_never initial_suspend() const
    {
        return {};
    }

    std::suspend_always final_suspend() const noexcept
    {
        return {};
    }

    std::suspend_always yield_value(T&& val)
    {
        if constexpr (std::is_move_assignable_v<T>) {
            value_ = std::move(val);
        }
        else {
            value_.emplace(std::move(val));
        }
        return {};
    }

    std::suspend_always yield_value(const T& val)
    {
        value_ = val;
        return {};
    }

    [[noreturn]]
    void unhandled_exception()
    {
        throw;
    }

    T& getValueRef(ez::utils::Badge<Generator::Iterator>)
    {
        assert(value_);
        return *value_;
    }

private:
    std::optional<T> value_;
};

template<typename T>
class Generator<T>::Iterator {
public:
    using iterator_concept = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = void;
    using reference = T&;

public:

    Iterator(std::coroutine_handle<Promise> coroutineHandle, ez::utils::Badge<Generator>)
        : coroutineHandle_{std::move(coroutineHandle)}
    {}

    Iterator& operator++()
    {
        assert(coroutineHandle_ && "Handle must be valid");
        coroutineHandle_.resume();
        return *this;
    }

    Iterator operator++(int)
    {
        assert(coroutineHandle_ && "Handle must be valid");
        coroutineHandle_.resume();
        return *this;
    }

    reference operator*()
    {
        return coroutineHandle_.promise().getValueRef(ez::utils::Badge<Iterator>{});
    }

    reference operator*() const
    {
        return coroutineHandle_.promise().getValueRef(ez::utils::Badge<Iterator>{});
    }

    bool operator == (std::default_sentinel_t) const noexcept
    {
        assert(coroutineHandle_ && "Handle must be valid");
        return coroutineHandle_.done();
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
    std::coroutine_handle<Promise> coroutineHandle_;
};

} // namespace ez::utils

#endif // EZ_UTILS_BADGE_H
