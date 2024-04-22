cmake_minimum_required(VERSION 3.16)

##### app tests #####

project(app_tests)

add_executable(app_tests app_tests.cpp)

target_link_libraries(app_tests core app)
