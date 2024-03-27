cmake_minimum_required(VERSION 3.16)

if (SGD_OS_EMSCRIPTEN)
    return()
endif ()

##### emgdb utility #####

project(emgdb)

add_executable(emgdb emgdb.cpp httplib.h )

target_link_libraries(emgdb core)

copy_output_file(emgdb ${CMAKE_SOURCE_DIR}/bin)
