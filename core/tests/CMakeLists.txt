cmake_minimum_required(VERSION 3.16)

##### core tests #####

project(core_tests)

add_executable(core_tests core_tests.cpp)
target_link_libraries(core_tests core)
