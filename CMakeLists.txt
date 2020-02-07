cmake_minimum_required(VERSION 3.0)

project(c++20_coroutines)

set(CMAKE_CXX_FLAGS "-fcoroutines-ts -stdlib=libc++")

set(CXX_STANDARD_REQUIRED ON)

add_subdirectory(generator)