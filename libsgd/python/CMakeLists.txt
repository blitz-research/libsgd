cmake_minimum_required(VERSION 3.16)

project(libsgd_python)

# 64 bit only for now
if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
	message(WARNING "### Python target only supported on 64 bit platforms, skipping...")
	return()
endif()

# Check we have SWIG
find_program(SWIG_PATH swig)
if(SWIG_PATH STREQUAL SWIG_PATH-NOTFOUND)
	message(WARNING "### Can't find swig program, skipping python target.")
	return()
endif()

if(SGD_OS_WINDOWS)
	SET(HOME $ENV{USERPROFILE})
else()
	SET(HOME $ENV{HOME})
endif()

set(PYTHON_DIR ${HOME}/AppData/Local/Programs/Python)

message("### Configuring python target...")

add_custom_command(
	COMMAND swig -python -outcurrentdir -I${CMAKE_SOURCE_DIR}/libsgd/include ${CMAKE_CURRENT_SOURCE_DIR}/sgd.i
	DEPENDS libsgd ${CMAKE_CURRENT_SOURCE_DIR}/sgd.i
	OUTPUT sgd_wrap.c sgd.py
	VERBATIM)

function(create_python_lib_target py_ver include_dirs libs)

	set(target libsgd_python_${py_ver})

	add_library(${target} SHARED sgd_wrap.c)
	target_include_directories(${target} PRIVATE ${include_dirs})
	target_link_libraries(${target} PRIVATE libsgd ${libs})

	set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libs/${py_ver})
	set_target_properties(${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libs/${py_ver})
	set_target_properties(${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libs/${py_ver})
	set_target_properties(${target} PROPERTIES OUTPUT_NAME _sgd)
	set_target_properties(${target} PROPERTIES PREFIX "")
	if(SGD_OS_WINDOWS)
		set_target_properties(${target} PROPERTIES SUFFIX .pyd)
	endif()

endfunction()

create_python_lib_target(py38 ${PYTHON_DIR}/Python38/include ${PYTHON_DIR}/Python38/libs/Python38.lib)
create_python_lib_target(py39 ${PYTHON_DIR}/Python39/include ${PYTHON_DIR}/Python39/libs/Python39.lib)
create_python_lib_target(py310 ${PYTHON_DIR}/Python310/include ${PYTHON_DIR}/Python310/libs/Python310.lib)
create_python_lib_target(py311 ${PYTHON_DIR}/Python311/include ${PYTHON_DIR}/Python311/libs/Python311.lib)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/pypi/ DESTINATION python)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/sgd.py DESTINATION python/libsgd)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libs DESTINATION python/libsgd FILES_MATCHING PATTERN "*.pyd")
