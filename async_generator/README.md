# `async_generator<T>`

`async_generator` is very similar to `generator` and can produce a sequence of
values lazily, but ‍`async_generator` is different from `generator` in that
values also can be produced asynchronously.
In the `generator` coroutine body, we only can use `co_yield` operator, but in
the `async_generator` coroutine body, we can use `co_yield` and `co_await`
operators.
Take a look at the following simple example.

```c++
cppcoro::async_generator<int> ticker(int count, threadpool& tp)
{
  for (int i = 0; i < count; ++i)
  {
    co_await tp.delay(std::chrono::seconds(1));
    co_yield i;
  }
}

cppcoro::task<> consumer(threadpool& tp)
{
  auto sequence = ticker(10, tp);
  for co_await(std::uint32_t i : sequence)
  {
    std::cout << "Tick " << i << std::endl;
  }
}
```

In the consumer task a `for co_await` range_based for-loop is used to consume
values of `sequence`. And also in the `ticker` a `co_await` operator is used to
waiting asynchronously.  
`async_generator` class has an iterator type and also `begin` and `end`
functions were implemented in order to be used in the range_based for-loop.

```c++
template<typename T>
class async_generator
{
public:

    class iterator
    {
        //...
    };

    Awaitable<iterator> begin() noexcept;
    iterator end() noexcept;
};
```

When the `begin` function is called, it returns an awaitable object. This
awaitable object must be awaited to get the iterator.

```c++
class iterator
{
public:
    //...
    Awaitable<iterator&> operator++() noexcept;
};
```

When the `iterator` is incremented, it returns an awaitable object. This
awaitable object must be awaited to resume the coroutine if the coroutine is
suspended.
If the coroutine runs to completion then the `iterator` will subsequently become
equal to the `end()` iterator. If the coroutine completes with an unhandled
exception then that exception will be rethrown from the `co_await` expression.
If you don’t know about awaitable objects, I recommend watching one of my
previous videos about implementing a generator, task class or recursive
generator. You can find these videos in my YouTube channel.
When the `async_generator` object is destructed it requests cancellation of the
underlying coroutine. If the coroutine has already run to completion or is
currently suspended in a `co_yield` expression then the coroutine is destroyed
immediately. Otherwise, the coroutine will continue execution until it either
runs to completion or reaches the next `co_yield` expression.  
Let's check two interesting test cases from `cppcoro`, it's very useful, it help
us to understand how to use `async_generator` and usages.

```c++
#include "doctest/doctest.h"

TEST_SUITE_BEGIN("async_generator");

TEST_CASE("async producer with async consumer"
 * doctest::description{
  "This test tries to cover the different state-transition code-paths\n"
  "- consumer resuming producer and producer completing asynchronously\n"
  "- producer resuming consumer and consumer requesting next value \
synchronously\n"
  "- producer resuming consumer and consumer requesting next value \
asynchronously" })
{
    // ...
}

TEST_SUITE_END();
```

The `doctest` framework is used to implement test cases. It's a single-header
testing framework for unit tests and TDD. It's very fast and feature-rich.
This test case shows a simple example of asynchronous programming by coroutines
and `task` class. This test case implements a simple producer-consumer scenario
by `async_generator`, `task` and `single_consumer_event`.  
Let's see the test body.

```c++
    cppcoro::single_consumer_event p1;
    cppcoro::single_consumer_event p2;
    cppcoro::single_consumer_event p3;
    cppcoro::single_consumer_event c1;
    //...
```

At first four objects from the `single_consumer_event` class are created to
synchronize producer and consumer. Three first objects (p1, p2 and p3) will be
used by the producer, and the last object (c1) will be used by the consumer.  

```c++
//...
auto produce = [&]() -> cppcoro::async_generator<std::uint32_t>
{
    co_await p1;
    co_yield 1;
    co_await p2;
    co_yield 2;
    co_await p3;
};
//...
```

Then the producer is implemented as a lambda function. The return type of lambda
function is an object from `async_generator` and it causes this lambda
function to turn into a coroutine. So it can use `co_await` and `co_yield`
operators in its body.