#include <experimental/filesystem>
#include <iostream>

#include "recursive_generator.hpp"
#include "generator.hpp"

namespace fs = std::experimental::filesystem;


// Lists the immediate contents of a directory.
cppcoro::generator<fs::directory_entry> list_directory(fs::path path)
{
    for(auto entry : fs::directory_iterator(path)) {
        co_yield entry;
    }
    co_return;
}

cppcoro::recursive_generator<fs::directory_entry> list_directory_recursive(fs::path path)
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
    auto fs_generator = list_directory_recursive(fs::path("/home/rarjmandi/Desktop"));
    for(const auto& elem : fs_generator){
      std::cout << elem << std::endl;
    }
    return 0;
}