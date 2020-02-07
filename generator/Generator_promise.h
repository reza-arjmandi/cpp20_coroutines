#pragma once

#include <experimental/coroutine>
#include <exception>

using namespace std::experimental;

template<typename T>
class Generator;

template<typename T>
class Generator_promise {

public:


    Generator<T> get_return_object()
    {
        using coro_type = coroutine_handle<Generator_promise<T>>;
        return Generator(coro_type::from_promise(*this));
    }

    suspend_always initial_suspend()
    {
        return {};
    }

    suspend_always final_suspend()
    {
        return {};
    }

    suspend_always yield_value(T value)
    {
        _value = value;
        return {};
    }

    suspend_never await_transform(T&& value) = delete;

    void return_void()
    {
    }

    void unhandled_exception()
    {
        _exception = std::current_exception();
    }

    void rethrow_if_exception()
    {
        if(_exception){
            std::rethrow_exception(_exception);
        }
    }

    T value() const
    {
        return _value;
    }

private:

    T _value;
    std::exception_ptr _exception;

};