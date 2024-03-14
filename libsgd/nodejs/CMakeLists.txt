cmake_minimum_required(VERSION 3.16)

project(libsgd_nodejs )

# No 32 bit nodejs
if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    return()
endif()

# Check we have SWIG
find_program(SWIG_PATH swig)
if(SWIG_PATH STREQUAL SWIG_PATH-NOTFOUND)
    message(WARNING "Can't find swig program, skipping nodejs target.")
    return()
endif()

# Check for npm
execute_process(COMMAND cmd /C npm --version RESULT_VARIABLE RV)
if(NOT RV EQUAL 0)
    message(WARNING "Can't find npm command, skipping nodejs target.")
    return()
endif()

if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/node_modules)

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/package.json ${CMAKE_CURRENT_BINARY_DIR})

    execute_process(
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMAND cmd /C npm update)

endif()

if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/binding.gyp)

    if(SGD_OS_WINDOWS)
        set(LIBSGD_STATIC_LIB ${CMAKE_BINARY_DIR}/libsgd/libsgd_static.lib)
    else()
        message(FATAL_ERROR "TODO")
    endif()

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/binding.gyp.template ${CMAKE_CURRENT_BINARY_DIR}/binding.gyp)

endif()

if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/build)

    execute_process(
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMAND cmd /C npx node-gyp configure)

endif()

set(output_file ${CMAKE_CURRENT_BINARY_DIR}/build/Release/libsgd.node)

add_custom_target(libsgd_nodejs ALL DEPENDS ${output_file})

add_custom_command(
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMAND swig -c++ -javascript -napi -outcurrentdir -I${CMAKE_SOURCE_DIR}/libsgd/include ${CMAKE_CURRENT_SOURCE_DIR}/libsgd.i
        COMMAND npx node-gyp build
        DEPENDS libsgd ${CMAKE_CURRENT_SOURCE_DIR}/libsgd.i
        OUTPUT ${output_file}
        VERBATIM)

if(SGD_INSTALL_DIR)
    install(FILES ${output_file} DESTINATION nodejs/${SGD_INSTALL_DIR})
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/helloworld.js DESTINATION nodejs/${SGD_INSTALL_DIR})
endif()
