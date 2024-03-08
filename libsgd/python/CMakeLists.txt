cmake_minimum_required(VERSION 3.16)

# No 32 bit python yet
if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    return()
endif()

# Hardcoded python sdk dir
if (SGD_OS_WINDOWS)
    set(PYTHON_SDK_DIR "C:/Users/marks/AppData/Local/Programs/Python/Python311")
else ()
    return()
endif ()

if (NOT EXISTS ${PYTHON_SDK_DIR})
    return()
endif ()

project(python-sgd)

add_library(python-sgd SHARED libsgd_wrap.c libsgd.py)

add_custom_command(
        COMMAND swig -python -outcurrentdir ${CMAKE_CURRENT_SOURCE_DIR}/libsgd.i
        OUTPUT libsgd_wrap.c libsgd.py
        MAIN_DEPENDENCY libsgd.i
        VERBATIM)

set_target_properties(python-sgd PROPERTIES OUTPUT_NAME _libsgd)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set_target_properties(python-sgd PROPERTIES SUFFIX .pyd)
endif ()

target_include_directories(python-sgd PRIVATE ${PYTHON_SDK_DIR}/include)

target_link_directories(python-sgd PRIVATE ${PYTHON_SDK_DIR}/libs)

target_link_libraries(python-sgd PRIVATE libsgd python311.lib)

if (SGD_INSTALL_DIR)
    install(TARGETS python-sgd DESTINATION python/${SGD_INSTALL_DIR})
endif()

#winpty python
