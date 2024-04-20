cmake_minimum_required(VERSION 3.16)

project(libsgd_python)

# 64 bit only for now
if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    return()
endif()

# Check we have SWIG
if(SWIG_PATH STREQUAL SWIG_PATH-NOTFOUND)
    message(WARNING "### Can't find swig program, skipping python target.")
    return()
endif()

# Check we have Python development
find_package(Python COMPONENTS Development)
if(NOT Python_Development_FOUND)
    message(WARNING "### Python development package not found, skipping python target")
    return()
endif()

message("### Configuring python target...")

add_library(libsgd_python SHARED sgd_wrap.c sgd.py sgd.i)

set_target_properties(libsgd_python PROPERTIES OUTPUT_NAME _sgd)
set_target_properties(libsgd_python PROPERTIES PREFIX "")

if (SGD_OS_WINDOWS)
    set_target_properties(libsgd_python PROPERTIES SUFFIX .pyd)
endif()

target_include_directories(libsgd_python PRIVATE ${Python_INCLUDE_DIRS})

target_link_libraries(libsgd_python PRIVATE libsgd ${Python_LIBRARIES})

add_custom_command(
        COMMAND swig -python -outcurrentdir -I${CMAKE_SOURCE_DIR}/libsgd/include ${CMAKE_CURRENT_SOURCE_DIR}/sgd.i
        DEPENDS libsgd ${CMAKE_CURRENT_SOURCE_DIR}/sgd.i
        OUTPUT sgd_wrap.c sgd.py
        VERBATIM)

if (SGD_INSTALL_DIR)
    if(SGD_OS_WINDOWS)
        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/_sgd.pyd DESTINATION python/${SGD_INSTALL_DIR})
    else()
        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/_sgd.so DESTINATION python/${SGD_INSTALL_DIR})
    endif()
    install(FILES
            ${CMAKE_CURRENT_BINARY_DIR}/sgd.py
            ${CMAKE_CURRENT_SOURCE_DIR}/helloworld.py
            DESTINATION python/${SGD_INSTALL_DIR})
endif()

#winpty python
