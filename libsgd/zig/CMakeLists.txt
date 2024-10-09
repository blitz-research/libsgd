cmake_minimum_required(VERSION 3.16)

# Requires python package: pycparser

project(libsgd_zig)

find_package(Python3 COMPONENTS Interpreter)
if (NOT Python3_FOUND)
	message(WARNING "### Can't find python3 interpreter, skipping zig target...")
	return()
endif ()

message("### Configuring zig target...")

set(SRC ${CMAKE_CURRENT_SOURCE_DIR})
set(BIN ${CMAKE_CURRENT_BINARY_DIR})
set(INC ${CMAKE_SOURCE_DIR}/libsgd/include)
set(OUT ${BIN}/sgd.zig)

add_custom_target(libsgd_zig ALL
	COMMAND ${Python3_EXECUTABLE} ${SRC}/apigen.py ${INC} ${OUT}
	DEPENDS libsgd ${SRC}/apigen.py ${SRC}/apigen_zig.py
	WORKING_DIRECTORY ${BIN}
	VERBATIM)

install(FILES ${OUT} ${SRC}/helloworld.zig DESTINATION zig)
