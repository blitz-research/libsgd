cmake_minimum_required(VERSION 3.16)

project(libsgd_zig)

find_package(Python3 COMPONENTS Interpreter)
if (NOT Python3_FOUND)
	message(WARNING "### Can't find python3 interpreter, skipping zig target...")
	return()
endif ()

set(SRC ${CMAKE_CURRENT_SOURCE_DIR})
set(BIN ${CMAKE_CURRENT_BINARY_DIR})
set(INC ${CMAKE_SOURCE_DIR}/libsgd/include)

set(OUTPUT_FILE ${BIN}/sgd.zig)

add_custom_command(
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	COMMAND ${Python3_EXECUTABLE} ${SRC}/bindgen.py ${INC} ${OUTPUT_FILE}
	OUTPUT ${OUTPUT_FILE}
	DEPENDS libsgd_dynamic
	VERBATIM)

add_custom_target(libsgd_zig ALL DEPENDS ${OUTPUT_FILE})

install(FILES ${OUTPUT_FILE} ${SRC}/helloworld.zig DESTINATION zig)
