#include <string>
#include <iostream>
#include <type_traits>
#include <thread>
#include <mutex>

#include "task.hpp"
#include "sync_wait.hpp"

task<std::string> make_word()
{
    co_return "hello";
}

task<std::string> make_hello()
{
    auto task = make_word();
    auto hello = co_await task;
    co_return hello + " world";
}

int main(int argc, char** argv)
{
    auto task = make_hello();
    std::cout <<  sync_wait(task) << std::endl;
    return 0;
}
