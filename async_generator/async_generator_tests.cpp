///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Lewis Baker
// Licenced under MIT license. See LICENSE.txt for details.
///////////////////////////////////////////////////////////////////////////////

#include <cppcoro/async_generator.hpp>
#include <cppcoro/single_consumer_event.hpp>
#include <cppcoro/task.hpp>
#include <cppcoro/sync_wait.hpp>
#include <cppcoro/when_all.hpp>

#include <ostream>
#include "doctest/doctest.h"

TEST_SUITE_BEGIN("async_generator");

TEST_CASE("async producer with async consumer"
	* doctest::description{
		"This test tries to cover the different state-transition code-paths\n"
		"- consumer resuming producer and producer completing asynchronously\n"
		"- producer resuming consumer and consumer requesting next value synchronously\n"
		"- producer resuming consumer and consumer requesting next value asynchronously" })
{
#if defined(_MSC_VER) && _MSC_FULL_VER <= 191025224 && defined(CPPCORO_RELEASE_OPTIMISED)
	FAST_WARN_UNARY_FALSE("MSVC has a known codegen bug under optimised builds, skipping");
	return;
#endif

	cppcoro::single_consumer_event p1;
	cppcoro::single_consumer_event p2;
	cppcoro::single_consumer_event p3;
	cppcoro::single_consumer_event c1;

	auto produce = [&]() -> cppcoro::async_generator<std::uint32_t>
	{
		co_await p1;
		co_yield 1;
		co_await p2;
		co_yield 2;
		co_await p3;
	};

	bool consumerFinished = false;

	auto consume = [&]() -> cppcoro::task<>
	{
		auto generator = produce();
		auto it = co_await generator.begin();
		CHECK(*it == 1u);
		(void)co_await ++it;
		CHECK(*it == 2u);
		co_await c1;
		(void)co_await ++it;
		CHECK(it == generator.end());
		consumerFinished = true;
	};

	auto unblock = [&]() -> cppcoro::task<>
	{
		p1.set();
		p2.set();
		c1.set();
		CHECK(!consumerFinished);
		p3.set();
		CHECK(consumerFinished);
		co_return;
	};

	cppcoro::sync_wait(cppcoro::when_all_ready(consume(), unblock()));
}

TEST_CASE("fmap")
{
	using cppcoro::async_generator;
	using cppcoro::fmap;

	auto iota = [](int count) -> async_generator<int>
	{
		for (int i = 0; i < count; ++i)
		{
			co_yield i;
		}
	};

	auto squares = iota(5) | fmap([](auto x) { return x * x; });

	cppcoro::sync_wait([&]() -> cppcoro::task<>
	{
		auto it = co_await squares.begin();
		CHECK(*it == 0);
		CHECK(*co_await ++it == 1);
		CHECK(*co_await ++it == 4);
		CHECK(*co_await ++it == 9);
		CHECK(*co_await ++it == 16);
		CHECK(co_await ++it == squares.end());
	}());
}

TEST_SUITE_END();
