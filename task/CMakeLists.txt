set(CMAKE_CXX_FLAGS "-fcoroutines-ts -stdlib=libc++")



set(CXX_STANDARD_REQUIRED ON)

add_executable(task main.cpp)
target_include_directories(task PUBLIC .)
set_property(TARGET task PROPERTY CXX_STANDARD 17)
