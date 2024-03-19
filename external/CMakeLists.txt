cmake_minimum_required(VERSION 3.16)

project(external)

include(FetchContent)

set(FETCHCONTENT_QUIET OFF)

##### stb #####

add_library(stb INTERFACE)

FetchContent_Declare(stb
        GIT_REPOSITORY https://github.com/blitz-research/stb.git
        GIT_TAG master
        GIT_SUBMODULES ""
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        EXCLUDE_FROM_ALL
        )

FetchContent_MakeAvailable(stb)

target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})

##### json11 #####

FetchContent_Declare(json11
        GIT_REPOSITORY https://github.com/blitz-research/json11.git
        GIT_TAG master
        GIT_SUBMODULES ""
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        EXCLUDE_FROM_ALL
        )

FetchContent_MakeAvailable(json11)

##### glfw #####

if (SGD_OS_EMSCRIPTEN)

    add_library(glfw INTERFACE)

    target_link_options(glfw INTERFACE -sUSE_GLFW=3)

else()

    FetchContent_Declare(glfw
            GIT_REPOSITORY https://github.com/blitz-research/glfw.git
            GIT_TAG master
            GIT_SUBMODULES ""
            GIT_SHALLOW TRUE
            GIT_PROGRESS TRUE
            EXCLUDE_FROM_ALL
            )

    FetchContent_MakeAvailable(glfw)

endif ()

##### tinygltf #####

FetchContent_Declare(tinygltf
        GIT_REPOSITORY https://github.com/blitz-research/tinygltf.git
        GIT_TAG master
        GIT_SUBMODULES ""
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        EXCLUDE_FROM_ALL
        )

#set(TINYGLTF_HEADER_ONLY ON)
set(TINYGLTF_BUILD_LOADER_EXAMPLE OFF)

FetchContent_MakeAvailable(tinygltf)

##### openxr_loader ####

#[[
if (SGD_OS_WINDOWS)

    FetchContent_Declare(openxr-sdk
            GIT_REPOSITORY "https://github.com/blitz-research/OpenXR-SDK.git"
            GIT_TAG main
            GIT_SUBMODULES ""
            GIT_SHALLOW TRUE
            EXCLUDE_FROM_ALL
            )

    FetchContent_MakeAvailable(openxr-sdk)

endif ()
]]

##### target: lib-dawn #####

add_library(lib-dawn INTERFACE)

if (SGD_OS_EMSCRIPTEN)

    target_link_options(lib-dawn INTERFACE -sUSE_WEBGPU=1)

else ()

    FetchContent_Declare(dawn
            GIT_REPOSITORY https://github.com/blitz-research/dawn.git
            #GIT_TAG openxr-dev
            GIT_TAG f9776999488fe1af3916d53f0a897113c0991567
            GIT_SUBMODULES ""
            GIT_SHALLOW TRUE
            GIT_PROGRESS TRUE
            EXCLUDE_FROM_ALL
            )

    set(DAWN_FETCH_DEPENDENCIES ON)
    set(DAWN_BUILD_SAMPLES OFF)
    set(DAWN_USE_GLFW OFF)

    if (SGD_OS_WINDOWS)
        set(DAWN_ENABLE_D3D12 ON)
        set(DAWN_ENABLE_D3D11 ON)
        set(DAWN_ENABLE_VULKAN ON)
    else ()
        set(ENABLE_OPENGLES ON)
        set(ENABLE_DESKTOP_GL ON)
        set(ENABLE_VULKAN ON)
    endif ()

    set(TINT_BUILD_CMD_TOOLS OFF)
    set(TINT_BUILD_DOCS OFF)
    set(TINT_BUILD_TESTS OFF)

    FetchContent_MakeAvailable(dawn)

    target_link_libraries(lib-dawn INTERFACE webgpu_dawn webgpu_cpp)

    target_include_directories(lib-dawn INTERFACE ${DAWN_INCLUDE_DIR})

endif ()
