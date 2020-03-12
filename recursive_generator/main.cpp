#include <experimental/filesystem>
#include <iostream>

#include "cppcoro/recursive_generator.hpp"
#include "cppcoro/generator.hpp"

namespace fs = std::filesystem;
using namespace cppcoro;

// Lists the immediate contents of a directory.
generator<fs::directory_entry> list_directory(fs::path path)
{
    for(auto entry : fs::directory_iterator(path)) {
        co_yield entry;
    }
    co_return;
}

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
      std::cout << elem.path() << std::endl;
    }
    return 0;
}