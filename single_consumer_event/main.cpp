#include <iostream>
#include <queue>
#include <numeric>
#include <exception>

#include "Sample1.h"
#include "Sample2.h"

template<typename T>
void assert_equal(const T& a, const T& b)
{
  if(a == b) {
    return;
  }
  
  class Assert_equal_exception : public std::exception {
    public:
    const char* what() const noexcept override 
    {
      return "assert_equal exception";
    }
  };

  throw Assert_equal_exception();
}

int main(int argc, char** argv)
{

  std::vector<int> data(1000000);
  std::iota(data.begin(), data.end(), 1);

  Sample1 sample1 {data};
  sample1.run();
  assert_equal(sample1.get_consumer_data(), data);
  auto sample1_elapsed = sample1.get_elapsed_time().count(); 
  std::cout 
    << "sample1 elapsed time : " 
    << sample1_elapsed 
    << " ns" << std::endl;

  Sample2 sample2 {data};
  sample2.run();
  assert_equal(sample2.get_consumer_data(), data);
  auto sample2_elapsed = sample2.get_elapsed_time().count(); 
  std::cout 
    << "sample2 elapsed time : " 
    << sample2_elapsed 
    << " ns" << std::endl;

  std::cout << "sample2 " 
    << ((sample1_elapsed - sample2_elapsed) / 
        static_cast<double>(sample1_elapsed)) * 100 
    << "% faster than sample1" << std::endl; 

  return 0;
}