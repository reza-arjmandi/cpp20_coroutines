find_package(Threads)
set(CXX_STANDARD_REQUIRED ON)
add_executable(single_consumer_event main.cpp lib/lightweight_manual_reset_event)
target_include_directories(single_consumer_event PUBLIC ${INCLUDE_DIRS})
target_link_libraries(single_consumer_event PUBLIC Threads::Threads)
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	target_link_libraries(single_consumer_event PUBLIC Synchronization)
endif()
set_property(TARGET single_consumer_event PROPERTY CXX_STANDARD 17)