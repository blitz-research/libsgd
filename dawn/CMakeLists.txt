cmake_minimum_required(VERSION 3.16)

include(FetchContent)

##### dawn #####

project(dawn)

set(SRCS
	copytexture.cpp
	copytexture.h
	copytexture.wgsl
	device.cpp
	device.h
	generatemipmaps.cpp
	generatemipmaps.h
	generatemipmaps.wgsl
	hellotriangle.cpp
	hellotriangle.h
	hellotriangle.wgsl
	shadermodule.cpp
	shadermodule.h

	exports.h
)

if (SGD_OS_MACOS)
	set(SRCS ${SRCS} device.mm)
endif ()

add_library(dawn ${SRCS})

target_link_libraries(dawn app lib-dawn)

add_subdirectory(tests)
