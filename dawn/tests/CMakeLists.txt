cmake_minimum_required(VERSION 3.16)

##### dawn tests #####

project(dawn_tests)

add_executable(dawn_tests dawn_tests.cpp)
target_link_libraries(dawn_tests dawn glfw)
