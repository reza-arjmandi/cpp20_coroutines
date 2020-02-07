#include <experimental/coroutine>

#include "Generator_promise.h"
#include "Generator_iterator.h"

using namespace std::experimental;

template<typename T>
class Generator {

public:

    using promise_type = Generator_promise<T>;
    using iterator = Generator_iterator<T>;

    Generator(coroutine_handle<promise_type> coroutine)
        : _coroutine{ coroutine }
    {}

    ~Generator()
    {
        if(_coroutine)
        {
            _coroutine.destroy();
        }
    }

    iterator begin()
    {
        _coroutine.resume();
        if(_coroutine.done()){
            _coroutine.promise().rethrow_if_exception();
        }
        return iterator{ _coroutine };
    }

private:

    coroutine_handle<promise_type> _coroutine;

};