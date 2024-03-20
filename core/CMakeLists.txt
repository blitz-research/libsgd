cmake_minimum_required(VERSION 3.16)

##### core module #####

project(core)

add_library(core
	appevent.cpp
	appevent.h
	bitmask.cpp
	bitmask.h
	condvar.cpp
	condvar.h
	config.cpp
	config.h
	expected.cpp
	expected.h
	exports.h
	fetch.cpp
	fetch.h
	fileio.cpp
	fileio.h
	log.cpp
	log.h
	object.cpp
	object.h
	path.cpp
	path.h
	property.cpp
	property.h
	random.cpp
	random.h
	shared.cpp
	shared.h
	sharedptr.cpp
	sharedptr.h
	signal.cpp
	signal.h
	std.cpp
	std.h
	stringutil.cpp
	stringutil.h
)

#target_precompile_headers(core PUBLIC exports.h)

if (SGD_OS_EMSCRIPTEN)
	target_link_options(core PUBLIC -sNO_EXIT_RUNTIME=1)
	#    target_link_options(core PUBLIC -sNO_EXIT_RUNTIME=1 -sDEFAULT_LIBRARY_FUNCS_TO_INCLUDE=['sgfRegistry','sgfRegisterObject','sgfGetObject','sgfDeregisterObject'])
	#    em_link_js_library(core "${PROJECT_SOURCE_DIR}/jsutils.js")
endif ()

target_link_libraries(core libcurl json11)

add_subdirectory(tests)
