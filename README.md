# cpp20_coroutines

## `generator<T>`

if you want to learn how to it works, you can see this video:  
[C++20 coroutines, implement generator in 5 minutes](https://www.youtube.com/watch?v=qmWLD5fr774&feature=youtu.be)
  
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