#include <thread>
#include <iostream>
#include <chrono>
#include <mutex>
#include <condition_variable>

#include <single_consumer_event.hpp>
#include <cppcoro/sync_wait.hpp>
#include <cppcoro/task.hpp>

#define with_coroutine

std::string value;
std::chrono::time_point<std::chrono::high_resolution_clock> start;
std::chrono::time_point<std::chrono::high_resolution_clock> end;
int number_of_test = 1000000;

#ifdef with_coroutine
  cppcoro::single_consumer_event event;

  cppcoro::task<> consumer()
  {
    // Coroutine will suspend here until some thread calls event.set()
    // eg. inside the producer() function below.
    co_await event;
    end = std::chrono::high_resolution_clock::now();
    // std::cout << "consumer got: " << value << std::endl;
  }

  void producer()
  {
    value = "foo";

    // This will resume the consumer() coroutine inside the call to set()
    // if it is currently suspended.
    start = std::chrono::high_resolution_clock::now();
    event.set();
  }

  void consume()
  {
      auto consumer_task = consumer();
      cppcoro::sync_wait(consumer_task);
  }

  int main(int argc, char ** argv)
  {
    auto sum = 0.0;
    for(auto i = 0; i < number_of_test; i++)
    {
      std::thread consume_thread{consume};
      std::thread produce_thread{producer};
      consume_thread.join();
      produce_thread.join();
      sum += std::chrono::duration_cast<std::chrono::nanoseconds>(
          end - start).count();
      event.reset();
    }
      

    std::cout << "average elapsed time: " << sum / number_of_test
         << " ns" << std::endl;
  }
#else

  std::mutex m;
  std::condition_variable cv;
  bool ready = false;
  bool processed = false;
  
  void consumer()
  {
    std::unique_lock<std::mutex> lk(m);
    cv.wait(lk, []{return ready;});
    end = std::chrono::high_resolution_clock::now();
    lk.unlock();
    cv.notify_one();
  }

  void producer()
  {
    {
      std::lock_guard<std::mutex> lk(m);
      value = "foo";
      ready = true;
    }
    start = std::chrono::high_resolution_clock::now();
    cv.notify_one();
  }

  int main(int argc, char ** argv)
  {
    auto sum = 0.0;
    for(auto i = 0; i < number_of_test; i++)
    {
      std::thread consume_thread{consumer};
      std::thread produce_thread{producer};
      consume_thread.join();
      produce_thread.join();
      sum += std::chrono::duration_cast<std::chrono::nanoseconds>(
          end - start).count();
      ready = false;
    }

    std::cout << "average elapsed time: " << sum / number_of_test
         << " ns" << std::endl;
  }
#endif