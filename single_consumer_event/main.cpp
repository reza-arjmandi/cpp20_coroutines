#include <iostream>
#include <queue>
#include <numeric>

#include "Sample1.h"
#include "Sample2.h"

int main(int argc, char** argv)
{

  std::vector<int> data{1000000000};
  std::iota(data.begin(), data.end(), 1);

  Sample1 sample1 {data};
  sample1.run();
  assert(sample1.get_consumer_data() == data);
  auto sample1_elapsed = sample1.get_elapsed_time().count(); 
  std::cout 
    << "sample1 elapsed time : " 
    << sample1_elapsed 
    << " ns" << std::endl;

  Sample2 sample2 {data};
  sample2.run();
  assert(sample2.get_consumer_data() == data);
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