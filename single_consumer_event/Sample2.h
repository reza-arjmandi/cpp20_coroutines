#pragma once

#include <cppcoro/single_consumer_event.hpp>
#include <cppcoro/sync_wait.hpp>
#include <cppcoro/task.hpp>

class Sample2 {

public:

  Sample2(std::vector<int>& producer_data):
    _producer_data(producer_data),
    _consumer_data(producer_data.size())
  {
  }

  void run()
  {
      std::thread consume_thread{&Sample2::consume, this};
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      std::thread produce_thread{&Sample2::producer, this};
      consume_thread.join();
      produce_thread.join();
  }

  std::vector<int> get_consumer_data() const
  {
    return _consumer_data;
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
      auto consumer_task = consumer();
      cppcoro::sync_wait(consumer_task);
  }

  void producer()
  {
    _start_time = std::chrono::high_resolution_clock::now();

    for(const auto& elem : _producer_data)
    {
      _shared_queue.push(elem);
      // This will resume the consumer() coroutine inside the call to set()
      // if it is currently suspended.
      _event.set();
    }
  }

  cppcoro::task<> consumer()
  {
    for(auto& elem : _consumer_data)
    {
      // Coroutine will suspend here until some thread calls event.set()
      // eg. inside the producer() function below.
      co_await _event;
      elem = _shared_queue.front();
      _shared_queue.pop();
      _event.reset();
    }
    
    _end_time = std::chrono::high_resolution_clock::now();
  }

  std::vector<int> _producer_data;
  std::vector<int> _consumer_data;
  std::queue<int> _shared_queue;
  cppcoro::single_consumer_event _event;
  std::chrono::time_point<std::chrono::high_resolution_clock> _start_time;
  std::chrono::time_point<std::chrono::high_resolution_clock> _end_time;

};