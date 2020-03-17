#pragma once

#include <cppcoro/single_consumer_async_auto_reset_event.hpp>
#include <cppcoro/when_all.hpp>
#include <cppcoro/sync_wait.hpp>
#include <cppcoro/task.hpp>
#include <sys/types.h>
#include <sys/syscall.h>
#include "Random_generator.h"

class Sample2 {

public:

  Sample2(std::size_t data_size):
    _data_size(data_size),
    _consumer_data(data_size)
  {
  }

  void run()
  {
    std::thread consume_thread{&Sample2::consume, this};
    std::thread produce_thread{&Sample2::produce, this};
    consume_thread.join();
    produce_thread.join();
  }

  std::vector<std::size_t> get_consumer_data() const
  {
    return _consumer_data;
  }

  std::vector<std::string> get_producer_data() const
  {
    return _producer_data;
  }

  std::chrono::nanoseconds get_elapsed_time()
  {
    auto result = std::chrono::duration_cast<std::chrono::nanoseconds>(
          _end_time - _start_time);
    return result;
  }

private:

  void consume()
  {
    cppcoro::sync_wait(consumer());
  }

  void produce()
  {
    cppcoro::sync_wait(producer());
  }

  cppcoro::task<> producer()
  {
    _start_time = std::chrono::high_resolution_clock::now();

    for(auto i = 0; i < _data_size; i++)
    {
      auto produced_value = _random_generator.generate_string(100);
      _shared_queue.push(produced_value);
      _producer_data.push_back(produced_value);
      _event.set();
    }
    co_return;
  }

  cppcoro::task<> consumer()
  {
    std::hash<std::string> hash;

    for(auto& elem : _consumer_data)
    {
      co_await _event;
      auto value = _shared_queue.front();
      _shared_queue.pop();
      elem = hash(value);
    }

    _end_time = std::chrono::high_resolution_clock::now();
  }

  std::vector<std::size_t> _consumer_data;
  std::vector<std::string> _producer_data;
  std::queue<std::string> _shared_queue;
  std::size_t _data_size;
  cppcoro::single_consumer_async_auto_reset_event _event;
  Random_generator _random_generator;
  std::chrono::time_point<std::chrono::high_resolution_clock> _start_time;
  std::chrono::time_point<std::chrono::high_resolution_clock> _end_time;

};