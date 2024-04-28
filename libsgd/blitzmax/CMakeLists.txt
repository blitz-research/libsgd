cmake_minimum_required(VERSION 3.16)

project(libsgd_blitzmax)

# 64 bit only for now
if (NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
	return()
endif ()

# Check we have sgd2bmx.exe
find_program(SGD2BMX sgd2bmx)
if (SGD2BMX STREQUAL SGD2BMX-NOTFOUND)
	message(WARNING "### Can't find sgd2bmx program, skipping blitzmax target.")
	return()
endif ()

message("### Configuring blitzmax target...")

add_custom_target(libsgd_blitzmax ALL
	COMMAND sgd2bmx ${CMAKE_SOURCE_DIR}/libsgd/include/sgd ${CMAKE_CURRENT_BINARY_DIR}/sgd.bmx
	DEPENDS libsgd_dynamic
	VERBATIM)

install(FILES
	${CMAKE_CURRENT_BINARY_DIR}/sgd.bmx
	${CMAKE_CURRENT_SOURCE_DIR}/helloworld.bmx
	DESTINATION blitzmax)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/examples DESTINATION blitzmax FILES_MATCHING PATTERN "*.bmx")
