cmake_minimum_required(VERSION 3.16)

# No 32 bit python support yet
if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    return()
endif()

find_package(Python COMPONENTS Development)
if(NOT Python_Development_FOUND)
    message(WARNING "Python development package not found, python target disabled")
    return()
endif()

project(libsgd_python)

add_library(libsgd_python SHARED libsgd_wrap.c libsgd.py libsgd.i)

set_target_properties(libsgd_python PROPERTIES OUTPUT_NAME _libsgd)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set_target_properties(libsgd_python PROPERTIES SUFFIX .pyd)
endif ()

target_include_directories(libsgd_python PRIVATE ${Python_INCLUDE_DIRS})

target_link_libraries(libsgd_python PRIVATE libsgd ${Python_LIBRARIES})

add_custom_command(
        COMMAND swig -python -outcurrentdir -I${CMAKE_CURRENT_SOURCE_DIR}/../include ${CMAKE_CURRENT_SOURCE_DIR}/libsgd.i
        DEPENDS libsgd ${CMAKE_CURRENT_SOURCE_DIR}/libsgd.i
        OUTPUT libsgd_wrap.c libsgd.py
        VERBATIM)

if (SGD_INSTALL_DIR)
    install(FILES
            ${CMAKE_CURRENT_BINARY_DIR}/libsgd.py
            ${CMAKE_CURRENT_BINARY_DIR}/_libsgd.pyd
            ${CMAKE_CURRENT_SOURCE_DIR}/helloworld.py
            DESTINATION python/${SGD_INSTALL_DIR})
endif()

#winpty python
