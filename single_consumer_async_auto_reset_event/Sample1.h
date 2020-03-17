#pragma once

#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>
#include <vector>
#include <functional>
#include "Random_generator.h"

class Sample1 {

public:

  Sample1(std::size_t data_size):
    _data_size(data_size),
    _consumer_data(data_size)
  {
  }

  void run()
  {
      std::thread consume_thread{&Sample1::consumer, this};
      std::thread produce_thread{&Sample1::producer, this};
      consume_thread.join();
      produce_thread.join();
  }

  std::vector<std::string> get_producer_data() const
  {
    return _producer_data;
  }

  std::vector<std::size_t> get_consumer_data() const
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

    for(auto i = 0; i < _data_size; i++)
    {
      std::lock_guard<std::mutex> lk(_mutex);
      auto produced_data = _random_generator.generate_string(100);
      _shared_queue.push(produced_data);
      _producer_data.push_back(produced_data);
    }
    
  }

  void consumer()
  {
    std::hash<std::string> hash;
    std::size_t idx = 0;
    while(idx != _data_size)
    {
      std::string value = "";
      {
        std::lock_guard<std::mutex> lk(_mutex);
        if(_shared_queue.size() > 0)
        {
          value = _shared_queue.front();
          _shared_queue.pop();
        }
      }
  
      if(value != "")
      {
        _consumer_data[idx++] = hash(value);
      }
    }
    
    _end_time = std::chrono::high_resolution_clock::now();
  }

  std::vector<std::size_t> _consumer_data;
  std::vector<std::string> _producer_data;
  std::queue<std::string> _shared_queue;
  std::size_t _data_size;
  std::mutex _mutex;
  Random_generator _random_generator;
  std::chrono::time_point<std::chrono::high_resolution_clock> _start_time;
  std::chrono::time_point<std::chrono::high_resolution_clock> _end_time;
};