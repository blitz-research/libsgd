cmake_minimum_required(VERSION 3.16)

##### window tests #####

project(window_tests)

if (SGD_OS_WINDOWS)
    add_executable(window_tests window_tests.cpp)
    target_link_libraries(window_tests window)
endif ()
