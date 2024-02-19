cmake_minimum_required(VERSION 3.16)

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

elseif (SGD_OS_WINDOWS)

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

set(TINYGLTF_BUILD_LOADER_EXAMPLE OFF)

FetchContent_MakeAvailable(tinygltf)

##### target: lib-dawn #####

add_library(lib-dawn INTERFACE)

if (SGD_OS_EMSCRIPTEN)

    target_link_options(lib-dawn INTERFACE -sUSE_WEBGPU=1)

else ()

    FetchContent_Declare(dawn
            GIT_REPOSITORY https://github.com/blitz-research/dawn.git
            GIT_TAG swapchain-fix
#            GIT_TAG openxr-dev
            GIT_SUBMODULES ""
            GIT_SHALLOW TRUE
            GIT_PROGRESS TRUE
            EXCLUDE_FROM_ALL
            )

    set(DAWN_FETCH_DEPENDENCIES ON)
    set(DAWN_BUILD_SAMPLES OFF)
    set(DAWN_USE_GLFW OFF)
    if (CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(DAWN_ENABLE_VULKAN OFF)
    endif ()

    set(TINT_BUILD_CMD_TOOLS OFF)
    set(TINT_BUILD_DOCS OFF)
    set(TINT_BUILD_TESTS OFF)

    #[[
    FetchContent_GetProperties(dawn)
    if(NOT dawn_POPULATED)
        # Fetch the content using previously declared details
        message("### FetchContent_Populate")
        FetchContent_Populate(dawn)

        # Set custom variables, policies, etc.
        # ...

        # Bring the populated content into the build
        message("### FetchContent add_subdirectory SOURCE_DIR:${dawn_SOURCE_DIR} BINARY_DIR:${dawn_BINARY_DIR}")

        add_subdirectory(${dawn_SOURCE_DIR} ${dawn_BINARY_DIR} EXCLUDE_FROM_ALL)
    endif()
    ]]

    FetchContent_MakeAvailable(dawn)

    target_link_libraries(lib-dawn INTERFACE webgpu_cpp webgpu_dawn dawn_native)

    target_include_directories(lib-dawn INTERFACE ${DAWN_INCLUDE_DIR})

endif ()

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
