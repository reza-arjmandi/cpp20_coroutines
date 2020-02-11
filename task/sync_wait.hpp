///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Lewis Baker
// Licenced under MIT license. See LICENSE.txt for details.
///////////////////////////////////////////////////////////////////////////////
#ifndef CPPCORO_SYNC_WAIT_HPP_INCLUDED
#define CPPCORO_SYNC_WAIT_HPP_INCLUDED

#include <sync_wait_task.hpp>

#include <cstdint>
#include <atomic>

	template<typename AWAITABLE>
	std::string sync_wait(AWAITABLE&& awaitable)
	{

		auto task = ::detail::make_sync_wait_task(std::forward<AWAITABLE>(awaitable));
		task.start();
		return task.result();
	}

#endif