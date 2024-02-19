cmake_minimum_required(VERSION 3.16)

##### libsgd #####

project(libsgd)

add_library(libsgd
        beginextern.h
        endextern.h
        graphics.cpp
        graphics.h
        internal.cpp
        internal.h
        libsgd.h
        window.cpp
        window.h
        types.cpp
        types.h
        )

target_link_libraries(libsgd window)

add_subdirectory(tests)
