cmake_minimum_required(VERSION 3.16)

##### xrsession #####

project(xrsession)

add_library(xrsession
        exports.h
        xrsession.cpp
        xrsession.h
        )

target_link_libraries(xrsession dawn openxr_loader lib-dawn)

add_subdirectory(tests)
