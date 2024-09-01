cmake_minimum_required(VERSION 3.16)

project(libsgd_python)

# 64 bit only for now
if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
	message(WARNING "### Python target only supported on 64 bit platforms, skipping python target.")
	return()
endif()

# Check we have SWIG
find_program(SWIG_PATH swig)
if(SWIG_PATH STREQUAL SWIG_PATH-NOTFOUND)
	message(WARNING "### Can't find swig program, skipping python target.")
	return()
endif()

set(PYTHON_DIR "")
set(CELLAR_DIR "")

# Check for SDK...
if(SGD_OS_WINDOWS)
	set(PYTHON_DIR $ENV{USERPROFILE}/AppData/Local/Programs/Python)
	if(NOT EXISTS ${PYTHON_DIR})
		message(WARNING "### Can't find ${PYTHON_DIR}, skipping python target.")
		return()
	endif()
elseif(SGD_OS_MACOS)
	set(CELLAR_DIR /opt/homebrew/Cellar)
	if(NOT EXISTS ${CELLAR_DIR})
		message(WARNING "### Can't find ${CELLAR_DIR} dir, skipping python target.")
		return()
	endif()
else()
	message(WARNING "### Python target only supported on Windows and MacOS, skipping.")
	return()
endif()

message("### Configuring python target...")

set(SRC ${CMAKE_CURRENT_SOURCE_DIR})
set(BIN ${CMAKE_CURRENT_BINARY_DIR})
set(INC ${CMAKE_SOURCE_DIR}/libsgd/include)

add_custom_command(
	COMMAND swig -python -outcurrentdir -I${INC} ${SRC}/sgd.i
	DEPENDS libsgd ${SRC}/sgd.i
	OUTPUT sgd_wrap.c sgd.py
	VERBATIM)

function(create_python_lib_target py_ver include_dirs libs)

	if(NOT EXISTS ${include_dirs} OR NOT EXISTS ${libs})
		message(WARNING "Python dev libs ${libs} for python version ${py_ver} not found, skipping version.")
		return()
	endif()

	set(target libsgd_python_${py_ver})

	add_library(${target} SHARED sgd_wrap.c)
	target_include_directories(${target} PRIVATE ${include_dirs})
	target_link_libraries(${target} PRIVATE libsgd ${libs})

	set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${BIN}/libs/${py_ver})
	set_target_properties(${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${BIN}/libs/${py_ver})
	set_target_properties(${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BIN}/libs/${py_ver})
	set_target_properties(${target} PROPERTIES OUTPUT_NAME _sgd)
	set_target_properties(${target} PROPERTIES PREFIX "")
	if(SGD_OS_WINDOWS)
		set_target_properties(${target} PROPERTIES SUFFIX .pyd)
	elseif(SGD_OS_MACOS)
			set_target_properties(${target} PROPERTIES SUFFIX .so)
	endif()

endfunction()

if(SGD_OS_WINDOWS)
	create_python_lib_target(py38 ${PYTHON_DIR}/Python38/include ${PYTHON_DIR}/Python38/libs/Python38.lib)
	create_python_lib_target(py39 ${PYTHON_DIR}/Python39/include ${PYTHON_DIR}/Python39/libs/Python39.lib)
	create_python_lib_target(py310 ${PYTHON_DIR}/Python310/include ${PYTHON_DIR}/Python310/libs/Python310.lib)
	create_python_lib_target(py311 ${PYTHON_DIR}/Python311/include ${PYTHON_DIR}/Python311/libs/Python311.lib)
elseif(SGD_OS_MACOS)
	set(FW_DIR Frameworks/Python.framework/Versions)
	set(SDK38_DIR ${CELLAR_DIR}/python@3.8/3.8.19/${FW_DIR}/3.8)
	create_python_lib_target(py38 ${SDK38_DIR}/include/python3.8 ${SDK38_DIR}/lib/libpython3.8.dylib)
endif()

install(FILES ${SRC}/helloworld.py DESTINATION python)

install(DIRECTORY ${SRC}/examples/ DESTINATION python/examples)

install(DIRECTORY ${SRC}/pypi-package/ DESTINATION python/pypi-package)
install(FILES ${BIN}/sgd.py DESTINATION python/pypi-package/libsgd)
install(DIRECTORY ${BIN}/libs DESTINATION python/pypi-package/libsgd FILES_MATCHING PATTERN "_sgd.*")
