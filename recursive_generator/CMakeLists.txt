set(CXX_STANDARD_REQUIRED ON)
add_executable(recursive_generator main.cpp)
target_include_directories(recursive_generator PUBLIC ${INCLUDE_DIRS})
set_property(TARGET recursive_generator PROPERTY CXX_STANDARD 17)