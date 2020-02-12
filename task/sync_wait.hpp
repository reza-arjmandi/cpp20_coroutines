#pragma once

#include <sync_wait_task.hpp>

#include <cstdint>
#include <atomic>

template<typename AWAITABLE>
auto sync_wait(AWAITABLE&& awaitable)
	-> typename awaitable_traits<AWAITABLE&&>::await_result_t
{
	auto task = ::detail::make_sync_wait_task(std::forward<AWAITABLE>(awaitable));
	task.start();
	return task.result();
}