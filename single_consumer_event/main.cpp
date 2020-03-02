#include <thread>
#include <iostream>

#include <single_consumer_event.hpp>
#include <cppcoro/sync_wait.hpp>
#include <cppcoro/task.hpp>

cppcoro::single_consumer_event event;
std::string value;

cppcoro::task<> consumer()
{
  // Coroutine will suspend here until some thread calls event.set()
  // eg. inside the producer() function below.
  co_await event;

  std::cout << "consumer got: " << value << std::endl;
}

void producer()
{
  value = "foo";

  // This will resume the consumer() coroutine inside the call to set()
  // if it is currently suspended.
  event.set();
}

void consume()
{
    auto consumer_task = consumer();
    cppcoro::sync_wait(consumer_task);
}

int main(int argc, char ** argv)
{
    auto consume_thread = std::thread(consume);

    std::cout << "press any key to start producer..." << std::endl;
    char ch;
    std::cin.get(ch);

    producer();

    consume_thread.join();
}