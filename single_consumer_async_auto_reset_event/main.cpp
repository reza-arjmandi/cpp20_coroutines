#include <iostream>
#include <queue>
#include <numeric>
#include <exception>

#include "Sample1.h"
#include "Sample2.h"

template<typename T>
void assert_equal(const T& a, const T& b)
{
  if(a==b) {
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
  std::size_t data_size(9000000);
  std::vector<std::size_t> expected_data(data_size);
  std::generate(expected_data.begin(), expected_data.end(), 
    [idx=0]()mutable{return std::hash<int>{}(idx++);});
  
  Sample1 sample1 {data_size};
  sample1.run();
  assert_equal(sample1.get_result_data(), expected_data);
  auto sample1_elapsed = sample1.get_elapsed_time().count(); 
  std::cout 
    << "sample1 elapsed time : " 
    << sample1_elapsed 
    << " ns" << std::endl;

  Sample2 sample2 {data_size};
  sample2.run();
  assert_equal(sample2.get_result_data(), expected_data);
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