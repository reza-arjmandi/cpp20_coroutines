#pragma once

#include <type_traits>

template<typename T>
struct awaitable_traits
{
	using awaiter_t = decltype(std::declval<T>().operator co_await());
	using await_result_t = decltype(std::declval<awaiter_t>().await_resume());
};