#pragma once

#include <type_traits>

template<typename T>
auto get_awaiter_impl(T&& value, int)
	noexcept(noexcept(static_cast<T&&>(value).operator co_await()))
	-> decltype(static_cast<T&&>(value).operator co_await())
{
	return static_cast<T&&>(value).operator co_await();
}

template<typename T>
auto get_awaiter(T&& value)
	noexcept(noexcept(get_awaiter_impl(static_cast<T&&>(value), 123)))
	-> decltype(get_awaiter_impl(static_cast<T&&>(value), 123))
{
	return get_awaiter_impl(static_cast<T&&>(value), 123);
}

template<typename T, typename = void>
struct awaitable_traits
{};

template<typename T>
struct awaitable_traits<T, std::void_t<decltype(get_awaiter(std::declval<T>()))>>
{
	using awaiter_t = decltype(get_awaiter(std::declval<T>()));
	using await_result_t = decltype(std::declval<awaiter_t>().await_resume());
};