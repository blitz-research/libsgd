cmake_minimum_required(VERSION 3.16)

##### window #####

project(window)

add_library(window
        exports.h
        window.cpp
        window.h
        )

target_link_libraries(window geom glfw)

add_subdirectory(tests)
