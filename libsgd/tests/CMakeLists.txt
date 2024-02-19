cmake_minimum_required(VERSION 3.16)

##### libsgd tests #####

project(libsgd_tests)

add_executable(libsgd_tests libsgd_tests.c)
target_link_libraries(libsgd_tests libsgd)
