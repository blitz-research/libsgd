cmake_minimum_required(VERSION 3.16)

##### dawn tests #####

project(dawn_tests)

add_executable(dawn_tests dawn_tests.cpp)
target_link_libraries(dawn_tests dawn glfw)

if (SGD_OS_WINDOWS)
    add_executable(vsync_test vsync_test.cpp)
    target_link_libraries(vsync_test lib-dawn glfw)
endif ()
