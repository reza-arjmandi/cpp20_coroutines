cmake_minimum_required(VERSION 3.0)

project(c++20_coroutines)
cmake_policy(SET CMP0054 NEW)
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	set(CMAKE_CXX_FLAGS "-fcoroutines-ts -stdlib=libc++")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	set(CMAKE_CXX_FLAGS "/await")
endif()

set(CXX_STANDARD_REQUIRED ON)
set(INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include)

add_subdirectory(generator)
add_subdirectory(task)
add_subdirectory(recursive_generator)
add_subdirectory(single_consumer_event)
add_subdirectory(single_consumer_async_auto_reset_event)
add_subdirectory(async_generator)