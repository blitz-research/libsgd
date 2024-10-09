cmake_minimum_required(VERSION 3.16)

# Requires python packages: sphinx, breathe, sphinx_tabs

project(libsgd_docs)

find_package(Doxygen)
if (NOT DOXYGEN_FOUND)
	message(WARNING "Can't find doxygen, skipping docs target...")
	return()
endif()

find_package(Python3 COMPONENTS Interpreter)
if (NOT Python3_FOUND)
	message(WARNING "### Can't find python3 interpreter, skipping docs target...")
	return()
endif ()

# TODO: Make surface we have sphinx and breathe packages.

set(SRC ${CMAKE_CURRENT_SOURCE_DIR})
set(BIN ${CMAKE_CURRENT_BINARY_DIR})
set(INC ${CMAKE_SOURCE_DIR}/libsgd/include)

# Configure Doxyfile
configure_file(${SRC}/Doxyfile ${BIN}/Doxyfile)

# Configure sphinx source dir, we need to do this update doxygen output xml path in conf.py
set(SOURCE_DIR ${SRC}/source)
set(TARGET_DIR ${BIN}/source)

file(MAKE_DIRECTORY ${TARGET_DIR})
file(MAKE_DIRECTORY ${TARGET_DIR}/_static)

configure_file(${SOURCE_DIR}/conf.py ${TARGET_DIR}/conf.py)

file(GLOB RST_FILES ${SOURCE_DIR}/*.rst)
file(GLOB CSS_FILES ${SOURCE_DIR}/_static/*)

add_custom_target(libsgd_docs ALL
	COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE} &&
			${CMAKE_COMMAND} -E copy ${RST_FILES} ${TARGET_DIR} &&
			${CMAKE_COMMAND} -E copy ${CSS_FILES} ${TARGET_DIR}/_static &&
			sphinx-build -b html ${BIN}/source ${BIN}/build
	WORKING_DIRECTORY ${BIN}
	VERBATIM
)

install(DIRECTORY ${BIN}/build/ DESTINATION html)
