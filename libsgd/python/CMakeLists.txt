cmake_minimum_required(VERSION 3.16)

project(libsgd_python)

# 64 bit only for now
if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
	message(WARNING "### Python target only supported on 64 bit platforms, skipping python target.")
	return()
endif()

find_package(Python3 COMPONENTS Interpreter)
if (NOT Python3_FOUND)
	message(WARNING "### Can't find python3 interpreter, skipping python target...")
	return()
endif ()

# Check we have SWIG
find_program(SWIG_PATH swig)
if(SWIG_PATH STREQUAL SWIG_PATH-NOTFOUND)
	message(WARNING "### Can't find swig program, skipping python target...")
	return()
endif()

set(PYTHON_DIR "")
set(CELLAR_DIR "")

# Check for Python SDKs...
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
elseif(SGD_OS_LINUX)
	set(PYTHON_CFG x86_64-linux-gnu)
else()
	message(WARNING "### Python target only supported on Windows and MacOS, skipping.")
	return()
endif()

message("### Configuring python target...")

set(SRC ${CMAKE_CURRENT_SOURCE_DIR})
set(BIN ${CMAKE_CURRENT_BINARY_DIR})
set(INC ${CMAKE_SOURCE_DIR}/libsgd/include)

add_custom_command(
	COMMAND ${Python3_EXECUTABLE} ${SRC}/genrenames.py ${INC} ${BIN}/renames.i &&
			swig -python -fcompact -O -outcurrentdir -I${INC} -I${BIN} ${SRC}/sgd.i
	DEPENDS libsgd ${SRC}/genrenames.py ${SRC}/sgd.i
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
	create_python_lib_target(py312 ${PYTHON_DIR}/Python312/include ${PYTHON_DIR}/Python312/libs/Python312.lib)
	create_python_lib_target(py313 ${PYTHON_DIR}/Python313/include ${PYTHON_DIR}/Python313/libs/Python313.lib)

elseif(SGD_OS_MACOS)
	set(FW_DIR Frameworks/Python.framework/Versions)

	#[[
	# I have no idea how to automate this...
	set(SDK38_DIR ${CELLAR_DIR}/python@3.8/3.8.20/${FW_DIR}/3.8)
	set(SDK39_DIR ${CELLAR_DIR}/python@3.9/3.9.20/${FW_DIR}/3.9)
	set(SDK310_DIR ${CELLAR_DIR}/python@3.10/3.10.15/${FW_DIR}/3.10)
	set(SDK311_DIR ${CELLAR_DIR}/python@3.11/3.11.10/${FW_DIR}/3.11)
	set(SDK312_DIR ${CELLAR_DIR}/python@3.12/3.12.6/${FW_DIR}/3.12)
	set(SDK313_DIR ${CELLAR_DIR}/python@3.13/3.13.0/${FW_DIR}/3.13)

	create_python_lib_target(py38 ${SDK38_DIR}/include/python3.8 ${SDK38_DIR}/lib/libpython3.8.dylib)
	create_python_lib_target(py39 ${SDK39_DIR}/include/python3.9 ${SDK39_DIR}/lib/libpython3.9.dylib)
	create_python_lib_target(py310 ${SDK310_DIR}/include/python3.10 ${SDK310_DIR}/lib/libpython3.10.dylib)
	create_python_lib_target(py311 ${SDK311_DIR}/include/python3.11 ${SDK311_DIR}/lib/libpython3.11.dylib)
	create_python_lib_target(py312 ${SDK312_DIR}/include/python3.12 ${SDK312_DIR}/lib/libpython3.12.dylib)
	create_python_lib_target(py312 ${SDK313_DIR}/include/python3.13 ${SDK312_DIR}/lib/libpython3.13.dylib)
	#]]

elseif(SGD_OS_LINUX)

	#[[
	create_python_lib_target(py38 /usr/include/python3.8 /usr/lib/python3.8/config-3.8-${PYTHON_CFG}/libpython3.8.a)
	create_python_lib_target(py39 /usr/include/python3.9 /usr/lib/python3.9/config-3.9-${PYTHON_CFG}/libpython3.9.a)
	create_python_lib_target(py310 /usr/include/python3.10 /usr/lib/python3.10/config-3.10-${PYTHON_CFG}/libpython3.10.a)
	create_python_lib_target(py311 /usr/include/python3.11 /usr/lib/python3.11/config-3.11-${PYTHON_CFG}/libpython3.11.a)
	create_python_lib_target(py312 /usr/include/python3.12 /usr/lib/python3.12/config-3.12-${PYTHON_CFG}/libpython3.12.a)
	create_python_lib_target(py313 /usr/include/python3.13 /usr/lib/python3.13/config-3.13-${PYTHON_CFG}/libpython3.13.a)
	#]]

endif()

install(FILES ${SRC}/helloworld.py DESTINATION python)

install(DIRECTORY ${SRC}/examples/ DESTINATION python/examples)

install(DIRECTORY ${SRC}/pypi-package/ DESTINATION python/pypi-package)
install(FILES ${BIN}/sgd.py DESTINATION python/pypi-package/libsgd)
install(DIRECTORY ${BIN}/libs DESTINATION python/pypi-package/libsgd FILES_MATCHING PATTERN "_sgd.*")
