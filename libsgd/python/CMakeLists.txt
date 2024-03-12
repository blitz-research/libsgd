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

project(libsgd_python)

add_library(libsgd_python SHARED libsgd_wrap.c libsgd.py)

set_target_properties(libsgd_python PROPERTIES OUTPUT_NAME _libsgd)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set_target_properties(libsgd_python PROPERTIES SUFFIX .pyd)
endif ()

add_custom_command(
        COMMAND swig -python -outcurrentdir ${CMAKE_CURRENT_SOURCE_DIR}/libsgd.i
        DEPENDS libsgd ${CMAKE_CURRENT_SOURCE_DIR}/libsgd.i
        OUTPUT libsgd_wrap.c libsgd.py
        VERBATIM)

target_include_directories(libsgd_python PRIVATE ${PYTHON_SDK_DIR}/include)

target_link_directories(libsgd_python PRIVATE ${PYTHON_SDK_DIR}/libs)

target_link_libraries(libsgd_python PRIVATE libsgd python311.lib)

if (SGD_INSTALL_DIR)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libsgd.py ${CMAKE_CURRENT_BINARY_DIR}/_libsgd.pyd DESTINATION python/${SGD_INSTALL_DIR})
#    install(TARGETS libsgd_python DESTINATION python/${SGD_INSTALL_DIR})
endif()

#winpty python
