#pragma once

#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>
#include <vector>
#include <functional>

class Sample1 {

public:

  Sample1(std::size_t data_size):
    _data_size(data_size),
    _result_data(data_size)
  {
  }

  void run()
  {
      std::thread consume_thread{&Sample1::consumer, this};
      std::thread produce_thread{&Sample1::producer, this};
      consume_thread.join();
      produce_thread.join();
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

  void producer()
  {
    _start_time = std::chrono::high_resolution_clock::now();

    for(auto i = 0; i < _data_size; i++)
    {
      std::lock_guard<std::mutex> lk(_mutex);
      _shared_queue.push(i);
    }
    
  }

  void consumer()
  {
    std::hash<int> hash;
    std::size_t idx = 0;
    while(idx != _data_size)
    {
      int value = -1;
      {
        std::lock_guard<std::mutex> lk(_mutex);
        if(_shared_queue.size() > 0)
        {
          value = _shared_queue.front();
          _shared_queue.pop();
        }
      }
  
      if(value != -1)
      {
        _result_data[idx++] = hash(value);
      }
    }
    
    _end_time = std::chrono::high_resolution_clock::now();
  }

  std::vector<std::size_t> _result_data;
  std::queue<int> _shared_queue;
  std::size_t _data_size;
  std::mutex _mutex;
  std::chrono::time_point<std::chrono::high_resolution_clock> _start_time;
  std::chrono::time_point<std::chrono::high_resolution_clock> _end_time;
};