set(CXX_STANDARD_REQUIRED ON)
add_executable(async_generator 
    main.cpp 
    async_generator_tests.cpp
    lib/lightweight_manual_reset_event)
target_include_directories(async_generator PUBLIC ${INCLUDE_DIRS})
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	target_link_libraries(async_generator PUBLIC Synchronization)
endif()
set_property(TARGET async_generator PROPERTY CXX_STANDARD 17)