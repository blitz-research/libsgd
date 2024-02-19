cmake_minimum_required(VERSION 3.16)

##### graphics #####

project(graphics)

add_library(graphics
        bindgroup.cpp
        bindgroup.h
        buffer.cpp
        buffer.h
        gltfloader.cpp
        gltfloader.h
        graphics.cpp
        graphics.h
        graphicsutil.cpp
        graphicsutil.h
        material.cpp
        material.h
        materialutil.cpp
        materialutil.h
        mesh.cpp
        mesh.h
        meshprimitives.cpp
        meshprimitives.h
        meshrenderer.cpp
        meshrenderer.h
        meshutil.cpp
        meshutil.h
        pbrmaterial.cpp
        pbrmaterial.h
        renderer.cpp
        renderer.h
        renderpipeline.cpp
        renderpipeline.h
        renderqueue.cpp
        renderqueue.h
        texture.cpp
        texture.h
        textureutil.cpp
        textureutil.h

        shaders/meshrenderer.wgsl
        shaders/pbrmaterial.wgsl
        shaders/scene.wgsl
        shaders/skybox.wgsl
        shaders/uniforms.h

        exports.h
        )

target_link_libraries(graphics window dawn tinygltf stb)

add_subdirectory(tests)
