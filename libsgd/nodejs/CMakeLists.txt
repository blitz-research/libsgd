cmake_minimum_required(VERSION 3.16)

# TODO: Fix weird issue where you can't just clean and build, you first need to remove nodejs cmake binary dir, then reconfigure and build.
# Only started after changing build from MinSizeRel to Release.

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

# Check we have npm - have to do it this weird way as it's a script, not an exe.
execute_process(COMMAND ${CMD} npm --version RESULT_VARIABLE RV)
if (NOT RV EQUAL 0)
	message(WARNING "### Can't find npm command, skipping nodejs target.")
	return()
endif ()

message("### Configuring nodejs target...")

set(SRC ${CMAKE_CURRENT_SOURCE_DIR})
set(BIN ${CMAKE_CURRENT_BINARY_DIR})
set(INC ${CMAKE_SOURCE_DIR}/libsgd/include)

# Install npm packages
configure_file(${SRC}/package.json ${BIN})
execute_process(
	WORKING_DIRECTORY ${BIN}
	COMMAND ${CMD} npm update)

# Generate binding.gyp
if (SGD_OS_WINDOWS)
	set(SGD_NODE_GYP_LIBS "\"${CMAKE_BINARY_DIR}/libsgd/sgd_static.lib\", \"crypt32.lib\", \"Wldap32.lib\", \"dxguid.lib\", \"Ws2_32.lib\"")
else ()
	set(SGD_NODE_GYP_LIBS "\"${CMAKE_BINARY_DIR}/libsgd/libsgd_static.a\"")
endif ()
configure_file(${SRC}/binding.gyp.template ${BIN}/binding.gyp)

if (SGD_CONFIG_DEBUG)
	set(NODEGYP_DEBUG --debug)
endif ()

# node-gyp configure
file(REMOVE_RECURSE ${BIN}/build)

execute_process(
	WORKING_DIRECTORY ${BIN}
	COMMAND ${CMD} npx node-gyp configure ${NODEGYP_DEBUG}
)

set(OUTPUT_FILE ${BIN}/build/Release/sgd.node)

add_custom_target(libsgd_nodejs ALL
	WORKING_DIRECTORY ${BIN}

	COMMAND	#${CMAKE_COMMAND} -E rm -rf ${BIN}/build &&
			#npx node-gyp configure ${NODEGYP_DEBUG} &&
			python ${SRC}/genrenames.py ${INC} ${BIN}/renames.i &&
			swig -c++ -javascript -napi -fcompact -O -outcurrentdir -I${INC} -I${BIN} ${SRC}/sgd.i &&
			npx node-gyp --silent build ${NODEGYP_DEBUG}
	DEPENDS libsgd_static ${SRC}/sgd.i ${SRC}/genrenames.py
	VERBATIM)

install(FILES ${OUTPUT_FILE} ${SRC}/helloworld.js DESTINATION nodejs)
