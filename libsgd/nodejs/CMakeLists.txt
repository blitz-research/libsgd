cmake_minimum_required(VERSION 3.16)

if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    return()
endif()

# Only if node-gyp configure has already been executed.
if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/build)
    message(WARNING "No nodejs build dir found, ignoring nodejs target. Have you run nodejs/init.sh?")
    return()
endif()

project(libsgd_nodejs)

set(output_file ${CMAKE_CURRENT_SOURCE_DIR}/build/Release/libsgd.node)

add_custom_target(libsgd_nodejs ALL DEPENDS ${output_file})

add_custom_command(
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND swig -c++ -javascript -napi -outcurrentdir ${CMAKE_CURRENT_SOURCE_DIR}/libsgd.i
        COMMAND node-gyp build
        DEPENDS libsgd ${CMAKE_CURRENT_SOURCE_DIR}/libsgd.i
        OUTPUT ${output_file}
        VERBATIM)

if(SGD_INSTALL_DIR)
    install(FILES ${output_file} DESTINATION nodejs/${SGD_INSTALL_DIR})
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/helloworld.js DESTINATION nodejs/${SGD_INSTALL_DIR})
endif()
