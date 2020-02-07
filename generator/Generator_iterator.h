#pragma once

#include <experimental/coroutine>

#include "Generator_promise.h"

using namespace std::experimental;

template<typename T>
class Generator_iterator {

public:

    using coroutine_handle_type = coroutine_handle<Generator_promise<T>>;

    Generator_iterator(coroutine_handle_type coroutine)
        : _coroutine{ coroutine }
    {}

    void operator++()
    {
        _coroutine.resume();
        if(_coroutine.done()) {
            _coroutine.promise().rethrow_if_exception();
        }
    }

    T operator*() const
    {
        return _coroutine.promise().value();
    }

private:

    coroutine_handle_type _coroutine;

};