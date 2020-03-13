#pragma once

#include <cppcoro/single_consumer_async_auto_reset_event.hpp>
#include <cppcoro/when_all.hpp>
#include <cppcoro/sync_wait.hpp>
#include <cppcoro/task.hpp>
#include "Random_generator.h"

class Sample2 {

public:

  Sample2(std::size_t data_size):
    _data_size(data_size),
    _result_data(data_size)
  {
  }

  void run()
  {
    cppcoro::sync_wait(cppcoro::when_all(consumer(), producer()));
  }

  std::vector<std::size_t> get_result_data() const
  {
    return _result_data;
  }

  std::chrono::nanoseconds get_elapsed_time()
  {
    auto result = std::chrono::duration_cast<std::chrono::nanoseconds>(
          _end_time - _start_time);
    return result;
  }

private:


  cppcoro::task<> producer()
  {
    _start_time = std::chrono::high_resolution_clock::now();

    for(auto i = 0; i < _data_size; i++)
    {
      _shared_queue.push(i);
      _event.set();
    }
    co_return;
  }

  cppcoro::task<> consumer()
  {
    std::hash<int> hash;
    for(auto& elem : _result_data)
    {
      co_await _event;
      auto value = _shared_queue.front();
      _shared_queue.pop();
      elem = hash(value);
    }

    _end_time = std::chrono::high_resolution_clock::now();
  }

  std::vector<std::size_t> _result_data;
  std::queue<int> _shared_queue;
  std::size_t _data_size;
  cppcoro::single_consumer_async_auto_reset_event _event;
  Random_generator _random_generator;
  std::chrono::time_point<std::chrono::high_resolution_clock> _start_time;
  std::chrono::time_point<std::chrono::high_resolution_clock> _end_time;

};