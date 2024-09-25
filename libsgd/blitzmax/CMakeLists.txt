cmake_minimum_required(VERSION 3.16)

project(libsgd_blitzmax)

find_package(Python3 COMPONENTS Interpreter)
if (NOT Python3_FOUND)
	message(WARNING "### Can't find python3 interpreter, skipping blitzmax target...")
	return()
endif ()

# Assumes pycparser module present

message("### Configuring blitzmax target...")

set(SRC ${CMAKE_CURRENT_SOURCE_DIR})
set(BIN ${CMAKE_CURRENT_BINARY_DIR})
set(INC ${CMAKE_SOURCE_DIR}/libsgd/include)
set(OUT ${BIN}/sgd.bmx)

add_custom_target(libsgd_blitzmax ALL
	COMMAND	${Python3_EXECUTABLE} ${SRC}/apigen.py ${INC} ${OUT}
	DEPENDS libsgd_dynamic ${SRC}/apigen.py ${SRC}/apigen_bmx.py
	WORKING_DIRECTORY ${BIN}
	VERBATIM)

install(FILES ${OUT} ${SRC}/helloworld.bmx DESTINATION blitzmax)

install(DIRECTORY ${SRC}/examples DESTINATION blitzmax FILES_MATCHING PATTERN "*.bmx")
