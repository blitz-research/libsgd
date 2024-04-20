cmake_minimum_required(VERSION 3.16)

project(libsgd_nodejs)

# 64 bit only for now
if (NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
	return()
endif ()

# Check we have SWIG
find_program(SWIG_PATH swig)
if (SWIG_PATH STREQUAL SWIG_PATH-NOTFOUND)
	message(WARNING "### Can't find swig program, skipping nodejs target.")
	return()
endif ()

if (SGD_OS_WINDOWS)
	set(CMD cmd /C)
else ()
	set(CMD "")
endif ()

# Check for npm
execute_process(COMMAND ${CMD} npm --version RESULT_VARIABLE RV)
if (NOT RV EQUAL 0)
	message(WARNING "### Can't find npm command, skipping nodejs target.")
	return()
endif ()

message("### Configuring nodejs target...")

# Install npm packages
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/package.json ${CMAKE_CURRENT_BINARY_DIR})
execute_process(
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	COMMAND ${CMD} npm update)

# Generate binding.gyp
if (SGD_OS_WINDOWS)
	set(SGD_NODE_GYP_LIBS "\"${CMAKE_BINARY_DIR}/libsgd/sgd_static.lib\", \"crypt32.lib\", \"Wldap32.lib\", \"dxguid.lib\", \"Ws2_32.lib\"")
else ()
	set(SGD_NODE_GYP_LIBS "\"${CMAKE_BINARY_DIR}/libsgd/libsgd_static.a\"")
endif ()
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/binding.gyp.template ${CMAKE_CURRENT_BINARY_DIR}/binding.gyp)

if (SGD_CONFIG_DEBUG)
	set(NODEGYP_DEBUG --debug)
endif ()

# node-gyp configure
execute_process(
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	COMMAND ${CMD} npx node-gyp configure ${NODEGYP_DEBUG}
)

set(output_file ${CMAKE_CURRENT_BINARY_DIR}/build/Release/sgd.node)

add_custom_target(libsgd_nodejs ALL DEPENDS ${output_file})

# node-gyp build
add_custom_command(
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	COMMAND swig -c++ -javascript -napi -outcurrentdir -I${CMAKE_SOURCE_DIR}/libsgd/include ${CMAKE_CURRENT_SOURCE_DIR}/sgd.i
	COMMAND npx node-gyp build ${NODEGYP_DEBUG}
	DEPENDS libsgd_static ${CMAKE_CURRENT_SOURCE_DIR}/sgd.i
	OUTPUT ${output_file}
	VERBATIM)

if (SGD_INSTALL_DIR)
	install(FILES ${output_file} DESTINATION nodejs/${SGD_INSTALL_DIR})
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/helloworld.js DESTINATION nodejs/${SGD_INSTALL_DIR})
endif ()
