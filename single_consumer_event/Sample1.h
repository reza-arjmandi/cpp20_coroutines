#pragma once

#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>
#include <vector>

class Sample1 {

public:

  Sample1(std::vector<int>& producer_data):
    _producer_data(producer_data),
    _consumer_data(producer_data.size())
  {
  }

  void run()
  {
      std::thread consume_thread{&Sample1::consumer, this};
      std::thread produce_thread{&Sample1::producer, this};
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

  void producer()
  {
    _start_time = std::chrono::high_resolution_clock::now();

    for(const auto& elem : _producer_data)
    {
      {
        _ready = false;
        std::lock_guard<std::mutex> lk(_mutex);
        _shared_queue.push(elem);
        _ready = true;
      }
      
      _condition_variable.notify_one();
    }
    
  }

  void consumer()
  {
    for(auto& elem : _consumer_data)
    {
      std::unique_lock<std::mutex> lk(_mutex);
      _condition_variable.wait(lk, [&]{return _ready;});
      elem = _shared_queue.front();
      _shared_queue.pop();
    }
    
    _end_time = std::chrono::high_resolution_clock::now();
  }

  std::vector<int> _producer_data;
  std::vector<int> _consumer_data;
  std::queue<int> _shared_queue;
  std::mutex _mutex;
  std::condition_variable _condition_variable;
  bool _ready = false;
  std::chrono::time_point<std::chrono::high_resolution_clock> _start_time;
  std::chrono::time_point<std::chrono::high_resolution_clock> _end_time;
};