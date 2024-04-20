cmake_minimum_required(VERSION 3.16)

project("LibSGD")

set(CMAKE_VERBOSE_MAKEFILE 0)

#  The OLD behavior for this policy is to proceed even when a normal variable of the same name exists. If the cache entry does not already exist and have a type then it is created and/or given a type and the normal variable is removed. The NEW behavior for this policy is to do nothing when a normal variable of the same name exists. The normal variable is not removed. The cache entry is not created or updated and is ignored if it exists.
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

# The OLD behavior of this policy is for find_package(PythonInterp) and find_package(PythonLibs) to load the deprecated modules. The NEW behavior is for uses of the modules to fail as if they do not exist.
set(CMAKE_POLICY_DEFAULT_CMP0148 OLD)

# The OLD behavior for this policy is for GIT_SUBMODULES when set to an empty string to initialize and update all git submodules. The NEW behavior for this policy is for GIT_SUBMODULES when set to an empty string to initialize and update no git submodules.
set(CMAKE_POLICY_DEFAULT_CMP0097 NEW)


# Options
option(SGD_NODEJS_TARGET_ENABLED "Nodejs target enabled" OFF)
option(SGD_PYTHON_TARGET_ENABLED "Python target enabled" OFF)

message("### Configuring LibSGD:")
message("### SGD_NODEJS_TARGET_ENABLED  ${SGD_NODEJS_TARGET_ENABLED}")
message("### SGD_PYTHON_TARGET_ENABLED  ${SGD_PYTHON_TARGET_ENABLED}")
message("### CMAKE_SYSTEM_NAME          ${CMAKE_SYSTEM_NAME}")
message("### CMAKE_BUILD_TYPE           ${CMAKE_BUILD_TYPE}")
message("### CMAKE_CXX_COMPILER_ID      ${CMAKE_CXX_COMPILER_ID}")
message("### CMAKE_SIZEOF_VOID_P        ${CMAKE_SIZEOF_VOID_P}")

# Config
#
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(SGD_CONFIG_DEBUG 1)
    add_compile_definitions(SGD_CONFIG_DEBUG=1)
else ()
    set(SGD_CONFIG_RELEASE 1)
    add_compile_definitions(SGD_CONFIG_RELEASE=1)
endif ()

# System
#
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(SGD_OS_WINDOWS 1)
    add_compile_definitions(SGD_OS_WINDOWS=1)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(SGD_OS_LINUX 1)
    add_compile_definitions(SGD_OS_LINUX=1)
elseif ((CMAKE_SYSTEM_NAME STREQUAL "MacOS") OR (CMAKE_SYSTEM_NAME STREQUAL "Darwin"))
    set(SGD_OS_MACOS 1)
    add_compile_definitions(SGD_OS_MACOS=1)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    set(SGD_OS_EMSCRIPTEN 1)
    add_compile_definitions(SGD_OS_EMSCRIPTEN=1)
else ()
    message(FATAL_ERROR "!!! Unrecognized system: ${CMAKE_SYSTEM_NAME}")
endif ()

# Compiler
#
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(SGD_COMPILER_MSVC 1)
    add_compile_definitions(SGD_COMPILER_MSVC=1)
elseif ((CMAKE_CXX_COMPILER_ID STREQUAL "Clang") OR (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang"))
    set(SGD_COMPILER_CLANG 1)
    add_compile_definitions(SGD_COMPILER_CLANG=1)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(SGD_COMPILER_GCC 1)
    add_compile_definitions(SGD_COMPILER_GCC=1)
else ()
    message(FATAL_ERROR "!!! Unsupported compiler: ${CMAKE_SYSTEM_NAME}")
endif ()

# Arch bit size
#
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(SGD_ARCH_64_BIT 1)
    add_compile_definitions(SGD_ARCH_64_BIT=1)
else()
    set(SGD_ARCH_32_BIT 1)
    add_compile_definitions(SGD_ARCH_32_BIT=1)
endif()

# Using C++ 17
#
set(CMAKE_CXX_STANDARD 17)

# Add/publish our root cmake dir
#
include_directories(${CMAKE_SOURCE_DIR})
add_compile_definitions(SGD_CMAKE_SOURCE_DIR=\"${CMAKE_SOURCE_DIR}\")

# Add cmake 'find' scripts
#
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

# Per compiler options
#
if (SGD_COMPILER_MSVC)

    # Static CRT
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

else ()

    # Use pthreads
    add_compile_options(-pthread)
    add_link_options(-pthread)

endif ()

# Per OS options
#
if (SGD_OS_LINUX)

    # Need to generate PIC code on Linux for dynamic libs.
    set(CMAKE_POSITION_INDEPENDENT_CODE 1)

    # Debug/optimizations
    If (SGD_CONFIG_RELEASE)
        add_compile_options(-O3)
    endif ()

    # Address sanitizer on Linux
    # Currently disabled as it breaks dynamic libs: FIXME
    #[[
    set(SANITIZER_OPTIONS -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer)
    add_compile_options(${SANITIZER_OPTIONS})
    add_link_options(${SANITIZER_OPTIONS})
    add_compile_definitions(SGD_ASAN_ENABLED=1)
    message("### Address sanitizer enabled: ${SANITIZER_OPTIONS}")
    ]]

elseif (SGD_OS_MACOS)

    # This defaults to "qc" but since macOS always runs ranlib right after, we can pass "Sqc" and avoid
    # the step of generating a symbol table. ranlib will do it for us.
    set(CMAKE_C_ARCHIVE_CREATE "<CMAKE_AR> Sqc <TARGET> <LINK_FLAGS> <OBJECTS>")
    set(CMAKE_CXX_ARCHIVE_CREATE "<CMAKE_AR> Sqc <TARGET> <LINK_FLAGS> <OBJECTS>")

    # -no_warning_for_no_symbols is a special flag in the macOS version of ranlib that stops it from
    # printing a warning about object files that don't contain any symbols. This warning is printed for
    # android/windows/etc files when building on macOS.
    set(CMAKE_C_ARCHIVE_FINISH "<CMAKE_RANLIB> -no_warning_for_no_symbols -c <TARGET>")
    set(CMAKE_CXX_ARCHIVE_FINISH "<CMAKE_RANLIB> -no_warning_for_no_symbols -c <TARGET>")

    # Debug/optimizations
    If (SGD_CONFIG_RELEASE)
        add_compile_options(-O3)
    endif ()

elseif (SGD_OS_EMSCRIPTEN)

    add_link_options(-fwasm-exceptions)
    add_link_options(-sINITIAL_MEMORY=256Mb -sSTACK_SIZE=64kb -sEXIT_RUNTIME=1)

    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    add_link_options(--emrun --shell-file "${CMAKE_SOURCE_DIR}/index_template.html")

    # Turns off interpreter
    add_link_options(-sDYNAMIC_EXECUTION=0)

    # Have to do this as EM_KEEPALIVE doesn't work with extern globals in archives
    #add_link_options(-sEXPORTED_FUNCTIONS=['_main','___heap_base','___stack_pointer'])

    # Debug/optimizations
    if (SGD_CONFIG_DEBUG)
        # Note: Install this chrome browser plugin for usable runtime debugging with -g option
        # https://chrome.google.com/webstore/detail/cc%20%20-devtools-support-dwa/pdcpmagijalfljmkmjngeonclgbbannb
        add_compile_options(-O1 -g2)
        add_link_options(-O1 -g2 --minify=0) # Don't minify javascript
    else ()
        add_compile_options(-O1)
        add_link_options(-O1)
    endif ()

endif ()

if (SGD_CONFIG_RELEASE AND CMAKE_SIZEOF_VOID_P EQUAL 8)

    if (SGD_OS_WINDOWS)
        set(SGD_INSTALL_OS windows)
    elseif (SGD_OS_LINUX)
        set(SGD_INSTALL_OS linux)
    elseif (SGD_OS_MACOS)
        set(SGD_INSTALL_OS macos)
	endif ()

	if(SGD_ARCH_64_BIT)
		set(SGD_INSTALL_ARCH x64)
	else()
		set(SGD_INSTALL_ARCH x86)
	endif()

	set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/cmake-install-${SGD_INSTALL_OS}-release)

	set(SGD_INSTALL_DIR ${SGD_INSTALL_OS}-${SGD_INSTALL_ARCH}-release)

endif ()

# Copy output exe/lib etc file to anywhere
#
function(copy_output_file target dst_dir)

    set(SRC $<TARGET_FILE:${target}>)
    set(DST ${dst_dir}/$<TARGET_FILE_BASE_NAME:${target}>$<TARGET_FILE_SUFFIX:${target}>)

    add_custom_command(TARGET ${target} POST_BUILD VERBATIM
            COMMAND ${CMAKE_COMMAND} -E echo "Copying ${target} output file \"${SRC}\" to \"${DST}\""
            COMMAND ${CMAKE_COMMAND} -E copy ${SRC} ${DST})
endfunction()

# Copy assets dir to output binary dir
#
function(copy_assets_to_binary_dir target)

    get_target_property(SRC_DIR ${target} SOURCE_DIR)
    set(SRC ${SRC_DIR}/assets)

    if (NOT EXISTS ${SRC})
        message(WARNING "Target assets dir ${SRC} does not exist.")
        return()
    endif ()

    if (SGD_OS_EMSCRIPTEN)
        target_link_options(${target} PUBLIC --preload-file "${SRC}@/assets")
        return()
    endif ()

    get_target_property(DST_DIR ${target} BINARY_DIR)
    set(DST ${DST_DIR}/assets)

    add_custom_command(TARGET ${target} POST_BUILD VERBATIM
            COMMAND ${CMAKE_COMMAND} -E echo "Copying assets dir \"${SRC}\" to \"${DST}\"."
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${SRC} ${DST})

endfunction()

add_subdirectory(external EXCLUDE_FROM_ALL)

add_subdirectory(core)          #
add_subdirectory(geom)          # core
add_subdirectory(dawn)          # geom
#add_subdirectory(xrsession)     # dawn
add_subdirectory(window)        # geom
add_subdirectory(graphics)      # window dawn
add_subdirectory(libsgd)        # graphics

#add_subdirectory(emgdb)
