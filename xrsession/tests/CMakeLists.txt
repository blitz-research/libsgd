cmake_minimum_required(VERSION 3.16)

##### xrsession tests #####

project(xrsession_tests)

add_executable(xrsession_tests xrsession_tests.cpp)

target_link_libraries(xrsession_tests xrsession)
