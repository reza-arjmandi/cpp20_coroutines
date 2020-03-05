# cpp20_coroutines

This repository is created in order to teach C++20 coroutines and usages.  
I've used [cppcoro](https://github.com/lewissbaker/cppcoro) and simplified it to clarify control flow of each classes.  
You can watch learning videos in my [youtube channel](https://www.youtube.com/channel/UCD63rKtTY3WoxaCE88Rg9AA).  
I appreciate [cppcoro](https://github.com/lewissbaker/cppcoro) for it's perfect implementation.

## `generator`

if you want to learn how it works, you can see this video:  
[C++20 coroutines, implement generator](https://youtu.be/VB6LsOhoUOE)
  
[slides](https://docs.google.com/presentation/d/1-tGOwPQdld-Xb6Mfqa0h7BrltTkh5ymiD9dzZfa6Epg/edit#slide=id.gc6f919934_0_0)
  
fibonacci example:

```c++
Generator<std::uint64_t> fibonacci()
{
    std::uint64_t a1 = 0, a2 = 1;
    for(;;){
        co_yield a2;
        std::uint64_t tmp = a1;
        a1 = a2;
        a2 = tmp + a2;
    }
}

int main(int argc, char ** argv)
{
    auto fib = fibonacci();
    auto fib_itr = fib.begin();
    
    for(;;){
        std::cout << *fib_itr << std::endl;
        ++fib_itr;
    }

    return 0;
}
```

## `task`

if you want to learn how it works, you can see this video:  
[C++20 coroutines, implement task](https://www.youtube.com/watch?v=WOczYq2oz50&t=1s)
  
[slides](https://docs.google.com/presentation/d/1qTD4ZCwkLvaBn7OTPBuM1IU7l2yTq2KvEoyhXcFLiPA/edit?usp=sharing)
  
task example:

```c++
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
```

## `recursive_generator`

if you want to learn how it works, you can see this video:  
[C++20 coroutines, implement recursive_generator](https://youtu.be/D1NFMEGQewg)
  
[slides](https://docs.google.com/presentation/d/1t0Dvw6kFCnSJYbOf1zJsRRdJxXYcLEnQMzJ_78VgQx8/edit?usp=sharing)
  
recursive_generator example:

```c++
generator<fs::directory_entry> list_directory(fs::path path);

recursive_generator<fs::directory_entry> list_directory_recursive(fs::path path)
{
  for (auto& entry : list_directory(path))
  {
    co_yield entry;
    if (fs::is_directory(entry))
    {
      co_yield list_directory_recursive(entry.path());
    }
  }
}

int main(int argc, char** argv)
{
    auto list_dirs = list_directory_recursive(fs::path("/home"));
    for(const auto& elem : list_dirs){
      std::cout << elem << std::endl;
    }
    return 0;
}
```

## `single_consumer_event`

if you want to learn how it works, you can see this video:  
[C++20 coroutines, synchronize producer-consumer threads by single_consumer_event](https://youtu.be/xmh2xaMtBHQ)
  
[slides](https://docs.google.com/presentation/d/1vYeDY_Yc5XJUVhVFkpRrYRxDJQFSbWvReD40PJD0dd8/edit?usp=sharing)
  
single_consumer_event example:

```c++
cppcoro::single_consumer_event event;
std::string value;

cppcoro::task<> consumer()
{
  // Coroutine will suspend here until some thread calls event.set()
  // eg. inside the producer() function below.
  co_await event;

  std::cout << value << std::endl;
}

void producer()
{
  value = "foo";

  // This will resume the consumer() coroutine inside the call to set()
  // if it is currently suspended.
  event.set();
}
```
