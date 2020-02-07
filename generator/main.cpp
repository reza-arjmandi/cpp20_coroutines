#include "Generator.h"

#include <cstdint>
#include <iostream>

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