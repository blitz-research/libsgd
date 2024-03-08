cmake_minimum_required(VERSION 3.16)

##### graphics #####

project(graphics)

add_library(graphics
        animation.cpp
        animation.h
        bindgroup.cpp
        bindgroup.h
        buffer.cpp
        buffer.h
        camera.cpp
        camera.h
        entity.cpp
        entity.h
        entityutil.cpp
        entityutil.h
        gltfloader.cpp
        gltfloader.h
        graphics.cpp
        graphics.h
        graphicsutil.cpp
        graphicsutil.h
        light.cpp
        light.h
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
        model.cpp
        model.h
        modelrenderer.cpp
        modelrenderer.h
        pbrmaterial.cpp
        pbrmaterial.h
        renderer.cpp
        renderer.h
        renderpipeline.cpp
        renderpipeline.h
        renderqueue.cpp
        renderqueue.h
        scene.cpp
        scene.h
        skinnedmeshrenderer.cpp
        skinnedmeshrenderer.h
        skinnedmodelrenderer.cpp
        skinnedmodelrenderer.h
        skybox.cpp
        skybox.h
        skyboxrenderer.cpp
        skyboxrenderer.h
        texture.cpp
        texture.h
        textureutil.cpp
        textureutil.h

        shaders/meshrenderer.wgsl
        shaders/pbrmaterial.wgsl
        shaders/scene.wgsl
        shaders/skinnedmeshrenderer.wgsl
        shaders/skyboxrenderer.wgsl
        shaders/uniforms.h

        exports.h
)

target_link_libraries(graphics window dawn tinygltf stb)

add_subdirectory(tests)
