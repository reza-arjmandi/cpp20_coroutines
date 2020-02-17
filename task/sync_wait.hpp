#pragma once

#include <cstdint>
#include <atomic>

#include "sync_wait_task.hpp"

template<typename AWAITABLE>
auto sync_wait(AWAITABLE&& awaitable)
{
	auto task = make_sync_wait_task(std::forward<AWAITABLE>(awaitable));
	task.start();
	return task.result();
}