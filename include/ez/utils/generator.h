#ifndef EZ_UTILS_GENERATOR_H
#define EZ_UTILS_GENERATOR_H

#include <cassert>
#include <coroutine>
#include <iterator>
#include <ranges>

#include "ez/utils/badge.h"

namespace ez::utils {

template<typename T>
class [[nodiscard]] Generator {
public:
    class Promise;
    class Iterator;
    using promise_type = Promise;

public:
    Generator(std::coroutine_handle<Promise> coroutine_handle, ez::utils::Badge<Promise>) noexcept
        : coroutine_handle_{std::move(coroutine_handle)}
    {}

    Generator(Generator&&) = default;
    Generator(const Generator&) = delete;
    Generator& operator = (Generator&&) = delete;
    Generator& operator = (const Generator&) = delete;
    ~Generator()
    {
        assert(coroutine_handle_);
        coroutine_handle_.destroy();
    }

    auto begin()
    {
        return Iterator{coroutine_handle_, ez::utils::Badge<Generator>{}};
    }

    auto end()
    {
        return std::default_sentinel;
    }

private:
    std::coroutine_handle<Promise> coroutine_handle_;
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

    void return_void() const noexcept
    {}

    [[noreturn]]
    void unhandled_exception()
    {
        throw;
    }

    [[nodiscard]]
    T& get_value_ref(ez::utils::Badge<Generator::Iterator>)
    {
        assert(value_);
        return *value_;
    }

private:
    // TODO: Read todo for Recursive_generator::value_ and do the same thing.
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

    Iterator(std::coroutine_handle<Promise> coroutine_handle, ez::utils::Badge<Generator>)
        : coroutine_handle_{std::move(coroutine_handle)}
    {}

    Iterator& operator++()
    {
        assert(coroutine_handle_ && "Handle must be valid");
        coroutine_handle_.resume();
        return *this;
    }

    void operator++(int)
    {
        operator++();
    }

    reference operator*() noexcept
    {
        return coroutine_handle_.promise().get_value_ref(ez::utils::Badge<Iterator>{});
    }

    reference operator*() const noexcept
    {
        return coroutine_handle_.promise().get_value_ref(ez::utils::Badge<Iterator>{});
    }

    bool operator == (std::default_sentinel_t) const noexcept
    {
        assert(coroutine_handle_ && "Handle must be valid");
        return coroutine_handle_.done();
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
    std::coroutine_handle<Promise> coroutine_handle_;
};

} // namespace ez::utils

#endif // EZ_UTILS_GENERATOR_H
