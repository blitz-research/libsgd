cmake_minimum_required(VERSION 3.16)

##### geom module #####

project(geom)

add_library(geom
        affinemat4.h
        affinemat4.inl
        box.h
        box.inl
        exports.h
        line.h
        line.inl
        mat3.h
        mat3.inl
        mat4.h
        mat4.inl
        plane.h
        plane.inl
        quat.h
        quat.inl
        rect.h
        rect.inl
        util.cpp
        util.h

        vec2.h
        vec2.inl
        vec3.h
        vec3.inl
        vec4.h
        vec4.inl
)

target_link_libraries(geom core)

add_subdirectory(tests)
