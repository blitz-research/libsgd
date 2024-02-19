cmake_minimum_required(VERSION 3.16)

include(FetchContent)

##### dawn #####

project(dawn)

add_library(dawn ${SOURCES}
        copytexture.cpp
        copytexture.h
        copytexture.wgsl
        device.cpp
        device.h
        generatemipmaps.cpp
        generatemipmaps.h
        generatemipmaps.wgsl
        hellotriangle.cpp
        hellotriangle.h
        hellotriangle.wgsl
        shadermodule.cpp
        shadermodule.h

        exports.h
        )

target_link_libraries(dawn geom lib-dawn)

add_subdirectory(tests)
