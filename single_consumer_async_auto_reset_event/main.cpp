#include <iostream>
#include <queue>
#include <numeric>
#include <exception>

#include "Sample1.h"
#include "Sample2.h"

void assert_equal(
  const std::vector<std::string>& producer_data, 
  const std::vector<std::size_t>& consumer_data)
{
  if(std::equal(producer_data.cbegin(), producer_data.cend(), consumer_data.cbegin(), 
  [hash = std::hash<std::string>()](const auto& elem1, const auto& elem2){
    return hash(elem1) == elem2;
  })){
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
  std::size_t data_size(1000000);
  
  Sample1 sample1 {data_size};
  sample1.run();
  assert_equal(sample1.get_producer_data(), sample1.get_consumer_data());
  auto sample1_elapsed = sample1.get_elapsed_time().count(); 
  std::cout 
    << "sample1 elapsed time : " 
    << sample1_elapsed 
    << " ns" << std::endl;

  Sample2 sample2 {data_size};
  sample2.run();
  assert_equal(sample2.get_producer_data(), sample2.get_consumer_data());
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